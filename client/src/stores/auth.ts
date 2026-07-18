import { ref } from 'vue'
import { defineStore } from 'pinia'

const STORAGE_KEY = 'auth'

function load() {
    const raw = localStorage.getItem(STORAGE_KEY)
    if (!raw) return { isLoggedIn: false, username: null as string | null }
    try {
        const { isLoggedIn, username } = JSON.parse(raw)
        return { isLoggedIn: !!isLoggedIn, username: username ?? null }
    } catch {
        return { isLoggedIn: false, username: null as string | null }
    }
}

function save(isLoggedIn: boolean, username: string | null) {
    localStorage.setItem(
        STORAGE_KEY,
        JSON.stringify({ isLoggedIn, username }),
    )
}

/**
 * 用户认证状态管理。
 *
 * 登录状态持久化到 localStorage
 */
export const useAuthStore = defineStore('auth', () => {
    const stored = load()
    const isLoggedIn = ref(stored.isLoggedIn)
    const username = ref<string | null>(stored.username)

    /**
     * 设置登录状态。
     * @param name 用户名，Key 登录时省略或传 null。
     */
    function login(name?: string | null) {
        username.value = name ?? null
        isLoggedIn.value = true
        save(true, username.value)
    }

    /**
     * 退出登录，清除认证状态。
     */
    function logout() {
        username.value = null
        isLoggedIn.value = false
        localStorage.removeItem(STORAGE_KEY)
    }

    return { isLoggedIn, username, login, logout }
})
