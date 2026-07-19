import { ref, watch } from 'vue'
import { defineStore } from 'pinia'

/** 亮/暗主题切换 */
export const useThemeStore = defineStore('theme', () => {
  const isDark = ref(localStorage.getItem('theme') === 'dark')

  watch(isDark, (val) => {
    localStorage.setItem('theme', val ? 'dark' : 'light')
    document.documentElement.classList.toggle('dark', val)
  }, { immediate: true })

  /** 切换亮色/暗色主题。 */
  function toggle() {
    isDark.value = !isDark.value
  }

  return { isDark, toggle }
})
