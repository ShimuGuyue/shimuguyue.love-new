<script setup lang="ts">
import { ref, computed, onMounted } from 'vue'
import { useAuthStore } from '@/stores/auth'
import { useThemeStore } from '@/stores/theme'

const auth = useAuthStore()
const theme = useThemeStore()

interface ImageItem {
  id: number
  path: string
  description: string
  scale: number
  rotation: number
  pos_x: number
  pos_y: number
}

const images = ref<ImageItem[]>([])
const permissions = ref<string[]>([])

const previewId = ref<number | null>(null)
const previewImage = computed(() =>
  previewId.value !== null
    ? images.value.find(i => i.id === previewId.value) ?? null
    : null
)

onMounted(async () => {
  await loadImages()
  if (auth.isLoggedIn && auth.id !== null) {
    try {
      const resp = await fetch(`/api/user/permissions?user_id=${auth.id}`)
      if (resp.ok) {
        const data = await resp.json()
        permissions.value = data.permissions || []
      }
    } catch { /* 静默 */ }
  }
})

async function loadImages() {
  try {
    const resp = await fetch('/api/images')
    if (resp.ok) images.value = await resp.json()
  } catch { /* 静默 */ }
}

function openPreview(id: number) {
  previewId.value = id
}
function closePreview() {
  previewId.value = null
}

function imgStyle(img: ImageItem) {
  return {
    left: `${img.pos_x}%`,
    top: `${img.pos_y}%`,
    transform: `translate(-50%, -50%) scale(${img.scale}) rotate(${img.rotation}deg)`,
  }
}
</script>

<template>
  <main class="home">
    <div class="home__layout">
      <div class="home__photo glass">
        <div
          v-for="img in images"
          :key="img.id"
          class="home__img"
          :style="imgStyle(img)"
        >
          <img
            :src="`/image/${img.path}`"
            :alt="img.description"
            draggable="false"
            @click.stop="openPreview(img.id)"
          />
        </div>
      </div>
      <div class="home__info"></div>
    </div>

    <div
      v-if="previewImage"
      class="home__preview"
      :style="{ background: theme.isDark ? 'rgba(0,0,0,0.01)' : 'rgba(255,255,255,0.01)' }"
      @click="closePreview"
    >
      <img :src="`/image/${previewImage.path}`" class="home__preview-img" />
    </div>
    <div v-if="previewImage" class="home__preview-desc glass">
      {{ previewImage.description || '暂无简介' }}
    </div>
  </main>
</template>

<style scoped>
.home {
  padding: 24px;
  height: calc(100vh - 80px);
  overflow: hidden;
}

.home__layout {
  height: 100%;
  display: grid;
  grid-template-columns: 3fr 2fr;
  gap: 24px;
}

.home__photo {
  position: relative;
  overflow: hidden;
  min-height: 100%;
}

.home__info {
  /* 右侧信息栏 */
}

.home__img {
  position: absolute;
  cursor: pointer;
}
.home__img img {
  display: block;
  max-width: 300px;
  max-height: 300px;
  border-radius: 4px;
  object-fit: contain;
  cursor: pointer;
  user-select: none;
}

.home__preview {
  position: fixed;
  inset: 0;
  z-index: 2000;
  backdrop-filter: blur(5px);
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: pointer;
}

.home__preview-img {
  max-height: 90vh;
  max-width: 65vw;
  object-fit: contain;
  border-radius: 8px;
}

.home__preview-desc {
  position: fixed;
  right: 32px;
  top: 50%;
  transform: translateY(-50%);
  z-index: 2001;
  padding: 24px;
  max-width: 260px;
  font-size: 1rem;
  color: var(--color-text);
  line-height: 1.8;
}
</style>

<style>
@import "@/assets/glass.css";
@import "@/assets/pink-theme.css";
</style>
