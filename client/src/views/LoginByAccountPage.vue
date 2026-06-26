<script setup lang="ts">
import { ref } from 'vue'
import { useRouter } from 'vue-router'
import { useAuthStore } from '../stores/auth'

const router = useRouter()
const auth = useAuthStore()
const username = ref('')
const password = ref('')
const submitting = ref(false)
const errorMsg = ref('')

async function handleSubmit()
{
  errorMsg.value = ''
  if (!username.value.trim() || !password.value) return

  submitting.value = true
  try
  {
    const res = await fetch('/api/login/account', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ username: username.value, password: password.value }),
    })
    const data = await res.json()

    if (data.success)
    {
      auth.setUser(data.user)
      router.push('/')
    }
    else
    {
      errorMsg.value = data.message || '登录失败'
    }
  }
  catch
  {
    errorMsg.value = '网络错误，请稍后重试'
  }
  finally
  {
    submitting.value = false
  }
}
</script>

<template>
  <main class="login-page">
    <div class="login-card">
      <h1 class="login-card__title">账号登录</h1>
      <p class="login-card__subtitle">输入你的用户名和密码</p>

      <form class="login-form" @submit.prevent="handleSubmit">
        <div class="login-form__group">
          <label class="login-form__label" for="username">用户名</label>
          <input
            id="username"
            v-model="username"
            class="login-form__input"
            type="text"
            placeholder="请输入用户名"
            autocomplete="username"
          />
        </div>

        <div class="login-form__group">
          <label class="login-form__label" for="password">密码</label>
          <input
            id="password"
            v-model="password"
            class="login-form__input"
            type="password"
            placeholder="请输入密码"
            autocomplete="current-password"
          />
        </div>

        <p
          v-if="errorMsg"
          style="color: #e74c3c; font-size: 0.14rem; text-align: center; margin: 0;"
        >{{ errorMsg }}</p>

        <button
          type="submit"
          class="login-form__submit"
          :disabled="submitting"
        >
          {{ submitting ? '登录中…' : '登录' }}
        </button>
      </form>

      <div class="login-card__footer">
        <RouterLink to="/login/key" class="login-card__link">
          尚未设置帐号？使用密钥登录
        </RouterLink>
      </div>
    </div>
  </main>
</template>

<style scoped>
/* 样式在 login.css */
</style>
