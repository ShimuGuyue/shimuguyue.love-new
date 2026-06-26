<script setup lang="ts">
import { useTheme } from '../composables/useTheme'
import { useAuthStore } from '../stores/auth'
import '../css/navbar.css'

const { isDark, toggle } = useTheme()
const auth = useAuthStore()
</script>

<template>
  <header class="header">
    <RouterLink to="/" class="header__logo">石木古月</RouterLink>

    <button
      class="header__theme-btn"
      @click="toggle"
      :style="{
        maskImage: `url(https://cdn.jsdelivr.net/npm/heroicons@2.1.1/24/solid/${isDark ? 'moon' : 'sun'}.svg)`,
      }"
    ></button>

    <RouterLink
      v-if="!auth.isLoggedIn"
      to="/login/key"
      class="header__login-btn"
    >权限认证</RouterLink>
    <RouterLink
      v-else
      to="/account"
      class="header__login-btn"
    >{{ auth.user?.username || '匿名用户' }}</RouterLink>

    <nav class="header__links">
      <RouterLink to="/" class="header__link">首页</RouterLink>
      <RouterLink to="/docs" class="header__link">文章</RouterLink>
      <RouterLink to="/projects" class="header__link">项目</RouterLink>
      <RouterLink to="/favorites" class="header__link">收藏夹</RouterLink>
      <RouterLink to="/about" class="header__link">关于我</RouterLink>
    </nav>

    <span class="header__separator"></span>

    <div class="header__social">
      <a href="https://github.com/ShimuGuyue" target="_blank" class="header__social-icon"></a>
    </div>
  </header>
</template>
