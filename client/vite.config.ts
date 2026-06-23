import { fileURLToPath, URL } from 'node:url'

import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import vueDevTools from 'vite-plugin-vue-devtools'

export default defineConfig({
  plugins: [
    // 配置 Vite 插件列表
    vue(),
    vueDevTools(),
  ],

  resolve: {
    // 配置模块解析规则
    alias: {
      // 配置路径别名
      '@': fileURLToPath(new URL('./src', import.meta.url)),
    },
  },
})
