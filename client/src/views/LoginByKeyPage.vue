<script setup lang="ts">
import { ref } from 'vue'

const key = ref('')
const submitting = ref(false)

function handleSubmit() {
  if (!key.value.trim()) return
  submitting.value = true
  // TODO: 后端接口对接
  console.log('key login:', key.value)
  setTimeout(() => {
    submitting.value = false
  }, 1000)
}
</script>

<template>
  <main class="login-page">
    <div class="login-card">
      <h1 class="login-card__title">密钥登录</h1>
      <p class="login-card__subtitle">粘贴密钥文本，即刻验证身份</p>

      <form class="login-form" @submit.prevent="handleSubmit">
        <div class="login-form__group">
          <label class="login-form__label" for="key">密钥</label>
          <textarea
            id="key"
            v-model="key"
            class="login-form__textarea"
            placeholder="请粘贴密钥"
          ></textarea>
        </div>

        <button
          type="submit"
          class="login-form__submit"
          :disabled="submitting"
        >
          {{ submitting ? '验证中…' : '登录' }}
        </button>
      </form>

      <div class="login-card__footer">
        <div class="login-card__divider"></div>
        <RouterLink to="/login/account" class="login-card__link">
          已创建帐号？密钥已失效？使用账号登录
        </RouterLink>
      </div>
    </div>
  </main>
</template>

<style scoped>
/* 样式在 login.css */
</style>
