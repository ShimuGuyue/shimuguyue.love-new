import { ref, watchEffect } from 'vue'

const THEME_KEY = 'theme'

// 读取本地存储或系统偏好
function getInitial(): 'light' | 'dark' {
  const stored = localStorage.getItem(THEME_KEY)
  if (stored === 'light' || stored === 'dark') return stored
  return window.matchMedia('(prefers-color-scheme: dark)').matches ? 'dark' : 'light'
}

const theme = ref<'light' | 'dark'>(getInitial())

// 同步到 DOM 和本地存储
watchEffect(() => {
  document.documentElement.setAttribute('data-theme', theme.value)
  localStorage.setItem(THEME_KEY, theme.value)
})

export function useTheme() {
  const isDark = ref(theme.value === 'dark')

  function toggle() {
    theme.value = theme.value === 'dark' ? 'light' : 'dark'
    isDark.value = theme.value === 'dark'
  }

  // 监听系统偏好变化
  const mq = window.matchMedia('(prefers-color-scheme: dark)')
  mq.addEventListener('change', (e) => {
    if (!localStorage.getItem(THEME_KEY)) {
      theme.value = e.matches ? 'dark' : 'light'
      isDark.value = theme.value === 'dark'
    }
  })

  return { isDark, toggle }
}
