import { createRouter, createWebHistory } from 'vue-router'
import HomePage from '../views/HomePage.vue'
import BlogPage from '../views/BlogPage.vue'
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

export default router
