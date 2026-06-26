import { ref, computed } from 'vue'
import { defineStore } from 'pinia'

interface User
{
  id: number
  username: string
}

/**
 * @brief 登录状态 store
 *
 * 登录成功后存储用户信息；登出或未登录时为 null。
 * 页面刷新后状态丢失（可后续扩展 localStorage 持久化）。
 */
export const useAuthStore = defineStore('auth', () =>
{
  const user = ref<User | null>(null)

  const isLoggedIn = computed(() => user.value !== null)

  /**
   * @brief 登录 — 存储用户信息
   *
   * @param u 后端返回的用户对象 { id, username }
   */
  function setUser(u: User)
  {
    user.value = { id: u.id, username: u.username }
  }

  /**
   * @brief 登出 — 清除用户信息
   */
  function logout()
  {
    user.value = null
  }

  return { user, isLoggedIn, setUser, logout }
})
