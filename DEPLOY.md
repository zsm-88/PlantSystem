# 部署指南

## Render 部署（免费）

### 1. 注册 Render
前往 [render.com](https://render.com)，用 GitHub 账号登录。

### 2. 创建 Web Service
1. 点击 **New +** → **Web Service**
2. 选择 `PlantSystem` 仓库
3. 填写配置：
   - **Name**: `plant-system`（或自定义）
   - **Branch**: `web`
   - **Root Directory**: 留空（根目录）
   - **Runtime**: `Node`
   - **Build Command**: 留空
   - **Start Command**: `node server.js`
   - **Plan**: **Free**

### 3. 配置环境变量
在 Render 的环境变量页面添加以下三项（值来自你本地的 `config.local.json`）：

| 变量名 | 说明 |
|---|---|
| `ONENET_PRODUCT_ID` | OneNet 产品 ID |
| `ONENET_DEVICE_NAME` | OneNet 设备名称 |
| `ONENET_ACCESS_KEY` | OneNet 访问密钥 |

### 4. 部署
点击 **Deploy**，等待构建完成后访问 Render 提供的域名即可。

---

## 其他方式

### Railway
1. 登录 [railway.app](https://railway.app)
2. 导入 `PlantSystem` 仓库，选择 `web` 分支
3. 设置相同的环境变量
4. 自动部署

### 本地运行
```bash
npm install  # 无需额外依赖，仅用于确认
node server.js
```
访问 `http://localhost:5713`（或设置的 PORT）。
