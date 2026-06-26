<script setup lang="ts">
import { ref } from 'vue'
import { useRouter } from 'vue-router'
import { useAuthStore } from '../stores/auth'

const router = useRouter()
const auth = useAuthStore()
const key = ref('')
const submitting = ref(false)
const errorMsg = ref('')

async function handleSubmit()
{
  errorMsg.value = ''
  if (!key.value.trim()) return

  submitting.value = true
  try
  {
    const res = await fetch('/api/login/key', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ key: key.value }),
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
      <h1 class="login-card__title">密钥登录</h1>
      <p class="login-card__subtitle">粘贴密钥文本，即刻验证身份</p>

      <form class="login-form" @submit.prevent="handleSubmit">
        <div class="login-form__group">
          <label class="login-form__label" for="key">密钥</label>
          <input
            id="key"
            v-model="key"
            class="login-form__input"
            type="text"
            placeholder="请粘贴密钥"
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
