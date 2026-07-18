import { ref } from 'vue'
import { defineStore } from 'pinia'

/**
 * 用户认证状态管理。
 *
 * 登录状态下持有当前用户的 username 和认证凭据；
 * 未登录时 isLoggedIn 为 false，username 为 null。
 */
export const useAuthStore = defineStore('auth', () => {
    const isLoggedIn = ref(false)
    const username = ref<string | null>(null)

    /**
     * 设置登录状态。
     * @param name 用户名，Key 登录时省略或传 null。
     */
    function login(name?: string | null) {
        username.value = name ?? null
        isLoggedIn.value = true
    }

    /**
     * 退出登录，清除认证状态。
     */
    function logout() {
        username.value = null
        isLoggedIn.value = false
    }

    return { isLoggedIn, username, login, logout }
})
