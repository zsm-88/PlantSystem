const crypto = require('crypto');
const fs = require('fs');
const http = require('http');
const https = require('https');
const path = require('path');
const { URL } = require('url');

const PORT = Number(process.env.PORT || 5173);
const localConfigPath = path.join(__dirname, 'config.local.json');
const localConfig = fs.existsSync(localConfigPath)
  ? JSON.parse(fs.readFileSync(localConfigPath, 'utf8'))
  : {};

const PRODUCT_ID = process.env.ONENET_PRODUCT_ID || localConfig.product_id || '8AV8W3KkCb';
const DEVICE_NAME = process.env.ONENET_DEVICE_NAME || localConfig.device_name || 'plant';
const ACCESS_KEY = process.env.ONENET_ACCESS_KEY || localConfig.access_key || '';
const API_HOST = 'iot-api.heclouds.com';

const MIME_TYPES = {
  '.html': 'text/html; charset=utf-8',
  '.js': 'text/javascript; charset=utf-8',
  '.css': 'text/css; charset=utf-8',
  '.json': 'application/json; charset=utf-8',
  '.png': 'image/png',
  '.svg': 'image/svg+xml; charset=utf-8',
  '.ico': 'image/x-icon',
};

function maskSecret(secret) {
  if (!secret) return '(empty)';
  if (secret.length <= 8) return '********';
  return `${secret.slice(0, 4)}...${secret.slice(-4)}`;
}

function makeToken() {
  if (!ACCESS_KEY) {
    throw new Error('Missing OneNET access key. Set ONENET_ACCESS_KEY or create config.local.json.');
  }

  const version = '2018-10-31';
  const method = 'sha1';
  const et = String(Math.floor(Date.now() / 1000) + 2 * 60 * 60);
  const res = `products/${PRODUCT_ID}`;
  const text = `${et}\n${method}\n${res}\n${version}`;
  const sign = crypto
    .createHmac(method, Buffer.from(ACCESS_KEY, 'base64'))
    .update(text)
    .digest('base64');

  return [
    `version=${version}`,
    `res=${encodeURIComponent(res)}`,
    `et=${et}`,
    `method=${method}`,
    `sign=${encodeURIComponent(sign)}`,
  ].join('&');
}

function sendJson(res, status, body) {
  res.writeHead(status, {
    'content-type': 'application/json; charset=utf-8',
    'cache-control': 'no-store',
  });
  res.end(JSON.stringify(body));
}

function readBody(req) {
  return new Promise((resolve, reject) => {
    let data = '';
    req.on('data', chunk => {
      data += chunk;
      if (data.length > 1024 * 1024) {
        reject(new Error('Request body too large'));
        req.destroy();
      }
    });
    req.on('end', () => resolve(data));
    req.on('error', reject);
  });
}

function callOneNet(method, apiPath, body) {
  return new Promise((resolve, reject) => {
    const payload = body ? JSON.stringify(body) : null;
    const startedAt = Date.now();
    const req = https.request({
      hostname: API_HOST,
      path: apiPath,
      method,
      headers: {
        Authorization: makeToken(),
        ...(payload ? {
          'content-type': 'application/json',
          'content-length': Buffer.byteLength(payload),
        } : {}),
      },
      timeout: 15000,
    }, response => {
      let data = '';
      response.setEncoding('utf8');
      response.on('data', chunk => { data += chunk; });
      response.on('end', () => {
        try {
          const parsed = data ? JSON.parse(data) : {};
          const duration = Date.now() - startedAt;
          console.log(`[OneNET] ${method} ${apiPath} -> ${response.statusCode} (${duration}ms)`);
          resolve({ status: response.statusCode || 500, body: parsed });
        } catch (error) {
          reject(new Error(`OneNET returned non-JSON response: ${data.slice(0, 120)}`));
        }
      });
    });

    req.on('timeout', () => req.destroy(new Error('OneNET request timed out')));
    req.on('error', error => {
      reject(new Error(`OneNET ${method} ${apiPath} failed: ${error.code || 'ERR'} ${error.message}`));
    });
    if (payload) req.write(payload);
    req.end();
  });
}

function serveStatic(req, res) {
  const url = new URL(req.url, `http://${req.headers.host}`);
  const requestedPath = decodeURIComponent(url.pathname);
  const relativePath = requestedPath === '/' ? 'index.html' : requestedPath.slice(1);
  const staticRoot = relativePath.startsWith('image/') ? path.resolve(__dirname, '..') : __dirname;
  const filePath = path.resolve(staticRoot, relativePath);

  if (!filePath.startsWith(staticRoot)) {
    res.writeHead(403);
    res.end('Forbidden');
    return;
  }

  fs.readFile(filePath, (error, content) => {
    if (error) {
      res.writeHead(404);
      res.end('Not found');
      return;
    }
    res.writeHead(200, {
      'content-type': MIME_TYPES[path.extname(filePath).toLowerCase()] || 'application/octet-stream',
      'cache-control': 'no-cache',
    });
    res.end(content);
  });
}

async function handleApi(req, res) {
  const url = new URL(req.url, `http://${req.headers.host}`);

  try {
    if (req.method === 'GET' && url.pathname === '/api/client-config') {
      sendJson(res, 200, {
        code: 0,
        msg: 'ok',
        data: {
          product_id: PRODUCT_ID,
          device_name: DEVICE_NAME,
          access_key: ACCESS_KEY,
          direct_get_url: `https://${API_HOST}/thingmodel/query-device-property`,
          direct_set_url: `https://${API_HOST}/thingmodel/set-device-property`,
        },
      });
      return;
    }

    if (req.method === 'GET' && url.pathname === '/api/health') {
      sendJson(res, 200, {
        code: 0,
        msg: 'ok',
        data: {
          product_id: PRODUCT_ID,
          device_name: DEVICE_NAME,
          access_key_configured: Boolean(ACCESS_KEY),
        },
      });
      return;
    }

    if (req.method === 'GET' && url.pathname === '/api/properties') {
      const productId = url.searchParams.get('product_id') || PRODUCT_ID;
      const deviceName = url.searchParams.get('device_name') || DEVICE_NAME;
      const apiPath = `/thingmodel/query-device-property?product_id=${encodeURIComponent(productId)}&device_name=${encodeURIComponent(deviceName)}`;
      const result = await callOneNet('GET', apiPath);
      sendJson(res, result.status, result.body);
      return;
    }

    if (req.method === 'POST' && url.pathname === '/api/properties/set') {
      const body = JSON.parse(await readBody(req) || '{}');
      const payload = {
        product_id: body.product_id || PRODUCT_ID,
        device_name: body.device_name || DEVICE_NAME,
        params: body.params || {},
      };
      const result = await callOneNet('POST', '/thingmodel/set-device-property', payload);
      sendJson(res, result.status, result.body);
      return;
    }

    sendJson(res, 404, { code: 404, msg: 'API not found' });
  } catch (error) {
    console.error(`[API ERROR] ${req.method} ${url.pathname}: ${error.message}`);
    sendJson(res, 500, { code: 500, msg: error.message });
  }
}

const server = http.createServer((req, res) => {
  if (req.url.startsWith('/api/')) {
    handleApi(req, res);
    return;
  }
  serveStatic(req, res);
});

server.listen(PORT, () => {
  console.log(`Plant dashboard: http://localhost:${PORT}`);
  console.log(`[Config] product_id=${PRODUCT_ID}, device_name=${DEVICE_NAME}, access_key=${maskSecret(ACCESS_KEY)}`);
  console.log(`[Health] http://localhost:${PORT}/api/health`);
});
