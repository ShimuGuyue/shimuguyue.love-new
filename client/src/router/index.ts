import { createRouter, createWebHistory } from 'vue-router'
import HomePage from '../views/HomePage.vue'
import DocsPage from '../views/DocsPage.vue'
import ProjectsPage from '../views/ProjectsPage.vue'
import FavoritesPage from '../views/FavoritesPage.vue'
import AboutPage from '../views/AboutPage.vue'
import AccountPage from '../views/AccountPage.vue'
import LoginPage from '../views/LoginPage.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'home',
      component: HomePage,
    },
    {
      path: '/docs',
      name: 'docs',
      component: DocsPage,
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
      path: '/login',
      name: 'login',
      component: LoginPage,
    },
  ],
})

export default router
