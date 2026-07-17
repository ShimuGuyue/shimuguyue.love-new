<script setup lang="ts">
import { storeToRefs } from 'pinia'
import { useThemeStore } from '@/stores/theme'

const theme = useThemeStore()
const { isDark } = storeToRefs(theme)
const { toggle } = theme
</script>

<template>
  <nav class="navbar">
    <RouterLink to="/" class="navbar-logo">石木古月</RouterLink>
    <button
      class="header__theme-btn"
      @click="toggle"
      :style="{
        maskImage: `url(https://cdn.jsdelivr.net/npm/heroicons@2.1.1/24/solid/${isDark ? 'moon' : 'sun'}.svg)`,
      }"
    ></button>

    <div class="navbar-spacer"></div>

    <div class="header__social">
      <a
        href="https://github.com/ShimuGuyue"
        target="_blank"
        class="header__social-icon"
        style="mask-image: url(https://cdn.jsdelivr.net/npm/simple-icons@11.15.0/icons/github.svg)"
      ></a>
    </div>
  </nav>
</template>

<style scoped>
.navbar {
  display: flex;
  align-items: center;
  height: 80px;
  padding: 0 32px;
  background-color: var(--color-nav-bg);
  border-bottom: 1px solid var(--color-border);
  box-shadow: 0 1px 4px rgba(0, 0, 0, 0.06);
  transition: background-color 0.3s, border-color 0.3s;
}

.navbar-logo {
  font-size: 1.8rem;
  font-weight: 700;
  color: var(--color-text);
  text-decoration: none;
  transition: color 0.3s;
}

.header__theme-btn {
  width: 24px;
  height: 24px;
  margin-left: 16px;
  border: none;
  background-color: var(--color-text-secondary);
  mask-size: contain;
  mask-repeat: no-repeat;
  mask-position: center;
  -webkit-mask-size: contain;
  -webkit-mask-repeat: no-repeat;
  -webkit-mask-position: center;
  cursor: pointer;
  transition: background-color 0.3s;
}

.header__theme-btn:hover {
  background-color: var(--color-text);
}

.navbar-spacer {
  flex: 1;
}

.header__social {
  display: flex;
  align-items: center;
  gap: 12px;
}

.header__social-icon {
  display: block;
  width: 24px;
  height: 24px;
  background-color: var(--color-text-secondary);
  mask-size: contain;
  mask-repeat: no-repeat;
  mask-position: center;
  -webkit-mask-size: contain;
  -webkit-mask-repeat: no-repeat;
  -webkit-mask-position: center;
  transition: background-color 0.3s;
}

.header__social-icon:hover {
  background-color: var(--color-text);
}
</style>
