<script setup lang="ts">
import { ref, watch } from 'vue'
import { useRoute } from 'vue-router'
import { useAuthStore } from './stores/auth'
import NavBar from './components/NavBar.vue'

const auth = useAuthStore()
const route = useRoute()

/* ========================================================================
   Toast — 路由拦截提示
   ======================================================================== */

const toastMsg = ref('')
const toastVisible = ref(false)
let toastTimer: ReturnType<typeof setTimeout> | null = null

/** 路由变化时消费拦截原因 */
watch(
  () => route.fullPath,
  () =>
  {
    const reason = auth.consumeRedirectReason()
    if (reason)
    {
      showToast(reason)
    }
  },
  { immediate: true },
)

function showToast(msg: string)
{
  if (toastTimer) clearTimeout(toastTimer)
  toastMsg.value = msg
  toastVisible.value = true
  toastTimer = setTimeout(() =>
  {
    toastVisible.value = false
  }, 4000)
}
</script>

<template>
  <NavBar />

  <!-- 全局 Toast -->
  <Transition name="toast">
    <div v-if="toastVisible" class="toast" @click="toastVisible = false">
      {{ toastMsg }}
    </div>
  </Transition>

  <RouterView />
</template>

<style>
/* ========================================================================
   Toast
   ======================================================================== */
.toast
{
  position: fixed;
  top: 0.9rem;
  left: 50%;
  transform: translateX(-50%);
  z-index: 9999;
  max-width: 5.6rem;
  padding: 0.12rem 0.24rem;
  font-size: 0.14rem;
  color: #fff;
  background: rgba(0, 0, 0, 0.82);
  border-radius: 0.06rem;
  text-align: center;
  line-height: 1.5;
  cursor: pointer;
  backdrop-filter: blur(8px);
  box-shadow: 0 0.04rem 0.16rem rgba(0, 0, 0, 0.3);
}

.toast-enter-active
{
  transition: all 0.25s ease;
}

.toast-leave-active
{
  transition: all 0.2s ease;
}

.toast-enter-from
{
  opacity: 0;
  transform: translateX(-50%) translateY(-0.12rem);
}

.toast-leave-to
{
  opacity: 0;
}
</style>