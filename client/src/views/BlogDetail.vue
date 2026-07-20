<script setup lang="ts">
import { ref, computed, onMounted } from 'vue'
import { useRoute } from 'vue-router'
import MarkdownIt from 'markdown-it'

const route = useRoute()

const md = new MarkdownIt({
  html: true,
  linkify: true,
  typographer: true,
})

interface BlogDetail {
  id: number
  title: string
  description: string | null
  content: string | null
  update_time: string
  category: string | null
  tags: string[]
}

const blog = ref<BlogDetail | null>(null)
const loading = ref(true)

const renderedContent = computed(() => {
  if (!blog.value?.content) return ''
  return md.render(blog.value.content)
})

onMounted(async () => {
  try {
    const fp = route.params.file_path as string
    const resp = await fetch('/api/blog?file_path=' + encodeURIComponent(fp))
    if (!resp.ok) throw new Error(`HTTP ${resp.status}`)
    blog.value = await resp.json()
  } catch (e) {
    console.error('获取博客失败:', e)
  } finally {
    loading.value = false
  }
})
</script>

<template>
  <main class="blog-detail">
    <p v-if="loading" class="blog-detail__status">加载中...</p>
    <p v-else-if="!blog" class="blog-detail__status">博客不存在</p>
    <div v-else class="blog-detail__layout">
      <!-- 左侧：元信息 -->
      <aside class="blog-detail__side">
        <h1 class="blog-detail__title">{{ blog.title }}</h1>
        <p v-if="blog.description" class="blog-detail__desc">{{ blog.description }}</p>
        <p v-if="blog.category" class="blog-category">{{ blog.category }}</p>
        <div class="blog-tags">
          <span
            v-for="tag in blog.tags"
            :key="tag"
            class="blog-tag"
          >{{ tag }}</span>
        </div>
        <time class="blog-detail__time">{{ blog.update_time }}</time>
      </aside>

      <!-- 中间：正文 -->
      <article v-if="blog.content" class="blog-detail__content" v-html="renderedContent"></article>

      <!-- 右侧留空 -->
      <div class="blog-detail__right"></div>
    </div>
  </main>
</template>

<style scoped>
.blog-detail {
  max-width: 1400px;
  margin: 0 auto;
  padding: 32px 24px 64px;
}

.blog-detail__status {
  text-align: center;
  padding: 64px 0;
  font-size: 1rem;
  color: var(--color-text-secondary);
}

.blog-detail__layout {
  display: grid;
  grid-template-columns: 1fr 3fr 1fr;
  gap: 40px;
}

/* ── 左侧 ── */

.blog-detail__side {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.blog-detail__title {
  margin: 0;
  font-size: 1.6rem;
  color: var(--color-text);
  line-height: 1.4;
}

.blog-detail__desc {
  margin: 0;
  font-size: 0.95rem;
  color: var(--color-text-secondary);
  line-height: 1.6;
}



.blog-detail__time {
  font-size: 0.8rem;
  color: var(--color-text-secondary);
}

/* ── 中间正文 ── */

.blog-detail__content {
  font-size: 1.05rem;
  color: var(--color-text);
  line-height: 1.8;
}
</style>

<style>
@import "@/assets/blog.css";
@import "@/assets/markdown.css";
</style>
