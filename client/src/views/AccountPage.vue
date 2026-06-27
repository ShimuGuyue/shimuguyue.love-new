<script setup lang="ts">
import { computed } from 'vue'
import { useRouter } from 'vue-router'
import { useAuthStore } from '../stores/auth'

const router = useRouter()
const auth = useAuthStore()

/* ========================================================================
   权限中文映射
   ======================================================================== */

const PERM_LABELS: Record<string, string> = {
  super_admin: '超级管理员'
}

function permLabel(perm: string): string
{
  return PERM_LABELS[perm] ?? perm
}

const permissions = computed<string[]>(() => auth.user?.permissions ?? [])

function handleLogout()
{
  auth.logout()
  router.push('/')
}
</script>

<template>
  <main class="login-page">
    <!-- 未登录 -->
    <div v-if="!auth.isLoggedIn" class="login-card" style="text-align: center;">
      <h1 class="login-card__title">个人中心</h1>
      <RouterLink
        to="/login/key"
        class="login-form__submit"
        style="display: inline-block; line-height: 0.44rem; text-decoration: none; margin-top: 0.16rem;"
      >去登录</RouterLink>
    </div>

    <!-- 已登录 -->
    <div v-else class="login-card">
      <h1 class="login-card__title">个人中心</h1>
      <p class="login-card__subtitle">{{ auth.user?.username || '匿名用户' }}</p>

      <div class="account-info">
        <div class="account-info__row">
          <span class="account-info__label">用户名</span>
          <span class="account-info__value">{{ auth.user?.username || '未设置' }}</span>
        </div>

        <div class="account-info__divider"></div>

        <div class="account-info__perm-header">
          <span class="account-info__label">拥有权限</span>
          <span class="account-info__perm-count">{{ permissions.length }} 项</span>
        </div>
        <ul class="account-info__perms">
          <li v-for="p in permissions" :key="p" class="account-info__perm-item">
            <span class="account-info__perm-dot"></span>
            <span class="account-info__perm-name">{{ permLabel(p) }}</span>
            <span class="account-info__perm-key">{{ p }}</span>
          </li>
        </ul>
      </div>

      <button class="login-form__submit" @click="handleLogout" style="margin-top: 0.08rem;">
        退出登录
      </button>
    </div>
  </main>
</template>

<style scoped>
/*
==================================================
  个人信息区
==================================================
*/

.account-info {
  display: flex;
  flex-direction: column;
  gap: 0.16rem;
  margin: 0.16rem 0 0;
  padding: 0.24rem;
  border-radius: 0.08rem;
  background: rgba(0, 0, 0, 0.03);
  border: 1px solid var(--nav-border);
}

[data-theme='dark'] .account-info {
  background: rgba(255, 255, 255, 0.04);
}

.account-info__row {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.account-info__label {
  font-size: 0.14rem;
  font-weight: 600;
  color: var(--nav-text);
}

.account-info__value {
  font-size: 0.15rem;
  color: var(--text);
  font-weight: 500;
}

.account-info__divider {
  height: 1px;
  background: var(--nav-border);
}

.account-info__perm-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.account-info__perm-count {
  font-size: 0.12rem;
  color: var(--nav-text);
  background: rgba(0, 0, 0, 0.06);
  padding: 0.02rem 0.08rem;
  border-radius: 0.04rem;
}

[data-theme='dark'] .account-info__perm-count {
  background: rgba(255, 255, 255, 0.08);
}

.account-info__perms {
  list-style: none;
  margin: 0;
  padding: 0;
  display: flex;
  flex-direction: column;
  gap: 0.08rem;
}

.account-info__perm-item {
  display: flex;
  align-items: center;
  gap: 0.08rem;
  font-size: 0.14rem;
}

.account-info__perm-dot {
  width: 0.06rem;
  height: 0.06rem;
  border-radius: 50%;
  background: var(--nav-active);
  flex-shrink: 0;
}

.account-info__perm-name {
  color: var(--text);
  font-weight: 500;
}

.account-info__perm-key {
  font-size: 0.11rem;
  color: var(--nav-text);
  font-family: 'Courier New', monospace;
}

.account-info__perm-empty {
  font-size: 0.13rem;
  color: var(--nav-text);
  margin: 0;
}
</style>
