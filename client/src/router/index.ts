import { createRouter, createWebHistory } from 'vue-router'
import Home from '@/views/Home.vue'
import Blogs from '@/views/Blogs.vue'
import Projects from '@/views/Projects.vue'
import Favorites from '@/views/Favorites.vue'
import About from '@/views/About.vue'
import LoginKey from '@/views/LoginKey.vue'
import LoginPassword from '@/views/LoginPassword.vue'
import Profile from '@/views/Profile.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    { path: '/', name: 'home', component: Home },
    { path: '/blogs', name: 'blogs', component: Blogs },
    { path: '/projects', name: 'projects', component: Projects },
    { path: '/favorites', name: 'favorites', component: Favorites },
    { path: '/about', name: 'about', component: About },
    { path: '/login/key', name: 'login-key', component: LoginKey },
    { path: '/login/password', name: 'login-password', component: LoginPassword },
    { path: '/profile', name: 'profile', component: Profile },
  ],
})

export default router
