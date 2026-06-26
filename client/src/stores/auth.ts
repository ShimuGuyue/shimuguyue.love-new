import { ref, computed } from 'vue'
import { defineStore } from 'pinia'

interface User
{
  id: number
  username: string
}

const STORAGE_KEY = 'auth_user'

function loadUser(): User | null
{
  try
  {
    const raw = localStorage.getItem(STORAGE_KEY)
    if (!raw) return null
    const parsed = JSON.parse(raw)
    if (parsed && typeof parsed.id === 'number' && typeof parsed.username === 'string')
      return { id: parsed.id, username: parsed.username }
    return null
  }
  catch
  {
    return null
  }
}

function saveUser(u: User): void
{
  try
  {
    localStorage.setItem(STORAGE_KEY, JSON.stringify(u))
  }
  catch { /* 存储满时静默忽略 */ }
}

function clearUser(): void
{
  try
  {
    localStorage.removeItem(STORAGE_KEY)
  }
  catch { /* 忽略 */ }
}

/**
 * @brief 登录状态 store（localStorage 持久化）
 *
 * 登录后用户信息写入 localStorage，刷新页面不丢失。
 */
export const useAuthStore = defineStore('auth', () =>
{
  const user = ref<User | null>(loadUser())

  const isLoggedIn = computed(() => user.value !== null)

  /**
   * @brief 登录 — 存储用户信息并持久化
   *
   * @param u 后端返回的用户对象 { id, username }
   */
  function setUser(u: User)
  {
    user.value = { id: u.id, username: u.username }
    saveUser(user.value)
  }

  /**
   * @brief 登出 — 清除用户信息及持久化数据
   */
  function logout()
  {
    user.value = null
    clearUser()
  }

  return { user, isLoggedIn, setUser, logout }
})
