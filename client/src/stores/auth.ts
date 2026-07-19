import { ref } from 'vue'
import { defineStore } from 'pinia'

const STORAGE_KEY = 'auth'

/**
 * 将认证状态持久化到 localStorage。
 * @param isLoggedIn  登录状态。
 * @param username    用户名。
 * @param permissions 权限列表。
 */
function save(isLoggedIn: boolean, username: string | null, permissions: string[]) {
    localStorage.setItem(
        STORAGE_KEY,
        JSON.stringify({ isLoggedIn, username, permissions }),
    )
}

/**
 * 从 localStorage 获取认证状态。
 * @returns 持久化的登录状态、用户名和权限列表。
 */
function load() {
    const raw = localStorage.getItem(STORAGE_KEY)
    if (!raw) return { isLoggedIn: false, username: null as string | null, permissions: [] as string[] }
    try {
        const { isLoggedIn, username, permissions } = JSON.parse(raw)
        return {
            isLoggedIn: !!isLoggedIn,
            username: username ?? null,
            permissions: Array.isArray(permissions) ? permissions : [],
        }
    } catch {
        return { isLoggedIn: false, username: null as string | null, permissions: [] as string[] }
    }
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
    const permissions = ref<string[]>(stored.permissions)

    /**
     * 设置登录状态。
     * @param name 用户名，未设置时省略或传 null。
     * @param perms 权限名列表。
     */
    function login(name?: string | null, perms?: string[]) {
        username.value = name ?? null
        permissions.value = perms ?? []
        isLoggedIn.value = true
        save(true, username.value, permissions.value)
    }

    /**
     * 退出登录，清除认证状态。
     */
    function logout() {
        username.value = null
        permissions.value = []
        isLoggedIn.value = false
        localStorage.removeItem(STORAGE_KEY)
    }

    return { isLoggedIn, username, permissions, login, logout }
})
