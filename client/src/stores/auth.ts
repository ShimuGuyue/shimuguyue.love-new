import { ref, computed } from 'vue'
import { defineStore } from 'pinia'

/* ========================================================================
   类型
   ======================================================================== */

interface User
{
  id: number
  username: string
  permissions: string[]
}

const STORAGE_KEY = 'auth_user'

/* ========================================================================
   持久化
   ======================================================================== */

function loadUser(): User | null
{
  try
  {
    const raw = localStorage.getItem(STORAGE_KEY)
    if (!raw) return null
    const parsed = JSON.parse(raw)
    if (
      parsed &&
      typeof parsed.id === 'number' &&
      typeof parsed.username === 'string' &&
      Array.isArray(parsed.permissions)
    )
      return { id: parsed.id, username: parsed.username, permissions: parsed.permissions as string[] }
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

/* ========================================================================
   Store
   ======================================================================== */

/**
 * @brief 登录状态 store（localStorage 持久化）
 *
 * 登录后用户信息及权限写入 localStorage，刷新页面不丢失。
 */
export const useAuthStore = defineStore('auth', () =>
{
  const user = ref<User | null>(loadUser())
  const redirectReason = ref<string>('')

  const isLoggedIn = computed(() => user.value !== null)

  const isSuperAdmin = computed(() =>
    user.value?.permissions.includes('super_admin') ?? false,
  )

  /**
   * @brief 登录 — 存储用户信息及权限并持久化
   *
   * @param u 后端返回 { id, username, permissions }
   */
  function setUser(u: User)
  {
    user.value = {
      id: u.id,
      username: u.username,
      permissions: u.permissions ?? [],
    }
    saveUser(user.value)
  }

  /**
   * @brief 登出 — 清除用户信息及持久化数据
   */
  function logout()
  {
    user.value = null
    redirectReason.value = ''
    clearUser()
  }

  /**
   * @brief 检查当前用户是否拥有指定权限
   */
  function hasPermission(perm: string): boolean
  {
    return user.value?.permissions.includes(perm) ?? false
  }

  /**
   * @brief 设置路由拦截原因（供 beforeEach 调用）
   */
  function setRedirectReason(msg: string)
  {
    redirectReason.value = msg
  }

  /**
   * @brief 读取并清除拦截原因（供 toast 消费）
   */
  function consumeRedirectReason(): string
  {
    const msg = redirectReason.value
    redirectReason.value = ''
    return msg
  }

  return { user, isLoggedIn, isSuperAdmin, redirectReason, setUser, logout, hasPermission, setRedirectReason, consumeRedirectReason }
})