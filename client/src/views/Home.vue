<script setup lang="ts">
import { ref, computed, onMounted, watch, nextTick } from 'vue'
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

const previewSrcRect = ref<DOMRect | null>(null)

watch(previewImage, async (img) => {
  if (!img || !previewSrcRect.value) return
  await nextTick()
  const el = document.querySelector('.home__preview-img') as HTMLElement | null
  if (!el) return
  const start = previewSrcRect.value
  const end = el.getBoundingClientRect()
  // 计算从墙上位置到屏幕中央的 delta（用 transform 偏移）
  const dx = (start.left + start.width / 2) - (end.left + end.width / 2)
  const dy = (start.top + start.height / 2) - (end.top + end.height / 2)
  const sx = start.width / end.width
  const sy = start.height / end.height
  el.animate([
    { transform: `translate(${dx}px, ${dy}px) scale(${sx}, ${sy}) rotate(${img.rotation}deg)`, opacity: 0 },
    { transform: 'translate(0, 0) scale(1, 1) rotate(0deg)', opacity: 1 },
  ], {
    duration: 450,
    easing: 'cubic-bezier(0.25, 0.46, 0.45, 0.94)',
    fill: 'backwards',
  })
})

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

const editMode = ref(false)
const draggingId = ref<number | null>(null)
const dragStart = ref({ x: 0, y: 0 })
/// 进入编辑模式时保存的快照
const editSnapshot = ref<string>('')

/// 点击空白处进入编辑模式
function onWallClick(e: MouseEvent) {
  if (editMode.value) return
  if ((e.target as HTMLElement).closest('.home__img')) return
  editMode.value = true
  // 保存快照
  editSnapshot.value = JSON.stringify(images.value.map(i => ({
    id: i.id, pos_x: i.pos_x, pos_y: i.pos_y, scale: i.scale, rotation: i.rotation,
  })))
}

async function exitEdit() {
  const changed = hasChanges()
  if (changed) {
    if (!permissions.value.includes('edit')) {
      alert('当前用户无 edit 权限，修改无法生效')
      revertChanges()
      editMode.value = false
      return
    }
    // 保存所有变更
    for (const img of images.value) {
      await saveMeta(img)
    }
  }
  editMode.value = false
}

function hasChanges(): boolean {
  try {
    const snap = JSON.parse(editSnapshot.value) as Pick<ImageItem, 'id' | 'pos_x' | 'pos_y' | 'scale' | 'rotation'>[]
    for (const img of images.value) {
      const s = snap.find(s => s.id === img.id)
      if (!s) return true
      if (s.pos_x !== img.pos_x || s.pos_y !== img.pos_y || s.scale !== img.scale || s.rotation !== img.rotation) return true
    }
    return false
  } catch { return false }
}

function revertChanges() {
  try {
    const snap = JSON.parse(editSnapshot.value) as Pick<ImageItem, 'id' | 'pos_x' | 'pos_y' | 'scale' | 'rotation'>[]
    for (const s of snap) {
      const img = images.value.find(i => i.id === s.id)
      if (img) {
        img.pos_x = s.pos_x
        img.pos_y = s.pos_y
        img.scale = s.scale
        img.rotation = s.rotation
      }
    }
  } catch { /* 静默 */ }
}

async function saveMeta(img: ImageItem) {
  await fetch('/api/image/save', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      'Authorization': `Bearer ${auth.token}`,
    },
    body: JSON.stringify({
      path: img.path,
      description: img.description,
      scale: img.scale,
      rotation: img.rotation,
      pos_x: img.pos_x,
      pos_y: img.pos_y,
    }),
  })
}

// ── 拖拽 ──
function onImgMouseDown(e: MouseEvent, imgId: number) {
  if (!editMode.value) return
  draggingId.value = imgId
  dragStart.value = { x: e.clientX, y: e.clientY }
  e.preventDefault()
  e.stopPropagation()
}

function onWallMouseMove(e: MouseEvent) {
  if (draggingId.value === null) return
  const wall = (e.currentTarget as HTMLElement).getBoundingClientRect()
  const img = images.value.find(i => i.id === draggingId.value)
  if (!img) return
  const dx = ((e.clientX - dragStart.value.x) / wall.width) * 100
  const dy = ((e.clientY - dragStart.value.y) / wall.height) * 100
  img.pos_x = Math.max(0, Math.min(100, img.pos_x + dx))
  img.pos_y = Math.max(0, Math.min(100, img.pos_y + dy))
  dragStart.value = { x: e.clientX, y: e.clientY }
}

function onWallMouseUp() {
  draggingId.value = null
}

// ── 滚轮缩放/旋转 ──
function onImgWheel(e: WheelEvent, imgId: number) {
  if (!editMode.value) return
  e.preventDefault()
  e.stopPropagation()
  const img = images.value.find(i => i.id === imgId)
  if (!img) return
  if (e.shiftKey) {
    img.rotation = (img.rotation + e.deltaY * 0.5 + 360) % 360
  } else {
    img.scale = Math.max(0.1, Math.min(5, img.scale - e.deltaY * 0.005))
  }
}

function openPreview(id: number, event: MouseEvent) {
  const el = event.currentTarget as HTMLElement
  previewSrcRect.value = el.getBoundingClientRect()
  previewId.value = id
}
function closePreview() {
  previewId.value = null
  previewSrcRect.value = null
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
      <div
        class="home__photo glass"
        @click="onWallClick"
        @mousemove="onWallMouseMove"
        @mouseup="onWallMouseUp"
        @mouseleave="onWallMouseUp"
      >
        <button v-if="editMode" class="home__edit-done" @click.stop="exitEdit">完成编辑</button>
        <div
          v-for="img in images"
          :key="img.id"
          class="home__img"
          :class="{ 'home__img--edit': editMode }"
          :style="imgStyle(img)"
          @mousedown="e => onImgMouseDown(e, img.id)"
        >
          <img
            :src="`/image/${img.path}`"
            :alt="img.description"
            draggable="false"
            @click.stop="!editMode && openPreview(img.id, $event)"
            @wheel.prevent="e => onImgWheel(e, img.id)"
          />
        </div>
      </div>
      <div class="home__info"></div>
    </div>

    <Transition name="preview">
      <div
        v-if="previewImage"
        class="home__preview"
        :style="{ background: theme.isDark ? 'rgba(0,0,0,0.01)' : 'rgba(255,255,255,0.01)' }"
        @click="closePreview"
      >
        <img
          :src="`/image/${previewImage.path}`"
          class="home__preview-img"
        />
      </div>
    </Transition>
    <Transition name="preview">
      <div v-if="previewImage" class="home__preview-desc glass">
        {{ previewImage.description || '暂无简介' }}
      </div>
    </Transition>
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

.home__img--edit {
  cursor: grab;
  outline: 2px dashed var(--pink-soft);
  outline-offset: 4px;
  border-radius: 4px;
}

.home__edit-done {
  position: absolute;
  top: 12px;
  right: 12px;
  z-index: 100;
  padding: 6px 16px;
  border: 1px solid var(--color-border);
  border-radius: 4px;
  background: rgba(0, 0, 0, 0.25);
  color: var(--color-text);
  font-size: 0.8rem;
  cursor: pointer;
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

/* Vue Transition 类 */
.preview-enter-active {
  transition: opacity 1s ease;
}
.preview-leave-active {
  transition: opacity 0.5s ease;
}
.preview-enter-from,
.preview-leave-to {
  opacity: 0;
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
