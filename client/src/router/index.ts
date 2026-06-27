import { createRouter, createWebHistory } from 'vue-router'
import { useAuthStore } from '../stores/auth'
import HomePage from '../views/HomePage.vue'
import BlogPage from '../views/BlogPage.vue'
import BlogDetailPage from '../views/BlogDetailPage.vue'
import BlogImportPage from '../views/BlogImportPage.vue'
import ProjectsPage from '../views/ProjectsPage.vue'
import FavoritesPage from '../views/FavoritesPage.vue'
import AboutPage from '../views/AboutPage.vue'
import AccountPage from '../views/AccountPage.vue'
import LoginByKeyPage from '../views/LoginByKeyPage.vue'
import LoginByAccountPage from '../views/LoginByAccountPage.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'home',
      component: HomePage,
    },
    {
      path: '/blogs',
      name: 'blogs',
      component: BlogPage,
    },
    {
      path: '/blogs/import',
      name: 'blog-import',
      component: BlogImportPage,
    },
    {
      path: '/blog/:title',
      name: 'blog-detail',
      component: BlogDetailPage,
    },
    {
      path: '/projects',
      name: 'projects',
      component: ProjectsPage,
    },
    {
      path: '/favorites',
      name: 'favorites',
      component: FavoritesPage,
    },
    {
      path: '/about',
      name: 'about',
      component: AboutPage,
    },
    {
      path: '/account',
      name: 'account',
      component: AccountPage,
    },
    {
      path: '/login/key',
      name: 'login-key',
      component: LoginByKeyPage,
    },
    {
      path: '/login/account',
      name: 'login-account',
      component: LoginByAccountPage,
    },
  ],
})

/**
 * @brief 全局前置守卫 — 权限校验
 */
router.beforeEach((to, _from, next) =>
{
  const required = to.meta.requiresPermission as string | undefined

  if (!required)
  {
    next()
    return
  }

  const auth = useAuthStore()

  if (!auth.isLoggedIn)
  {
    auth.setRedirectReason(`访问 ${to.path} 需要登录，请先登录`)
    next({ name: 'login-key' })
    return
  }

  if (!auth.hasPermission(required))
  {
    auth.setRedirectReason(`你的帐号没有「${required}」权限，无法访问 ${to.path}`)
    next({ name: 'home' })
    return
  }

  next()
})

export default router