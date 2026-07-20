<script setup lang="ts">
import { ref, computed, onMounted, nextTick, watch } from 'vue'
import { useRoute } from 'vue-router'
import MarkdownIt from 'markdown-it'
import taskLists from 'markdown-it-task-lists'
import hljs from 'highlight.js'
import 'highlight.js/styles/github.css'

const route = useRoute()

const md = new MarkdownIt({
  html: true,
  linkify: true,
  typographer: true,
}).use(taskLists, { enabled: true, label: true, labelAfter: true })

/** 自定义 fence 渲染：在代码块上显示语言标签 */
const rawFence = md.renderer.rules.fence!
md.renderer.rules.fence = (tokens, idx, options, env, self): string => {
  const token = tokens[idx]!
  const lang = token.info.trim().split(/\s+/)[0] ?? ''
  const body = rawFence(tokens, idx, options, env, self)
  if (!lang) return body
  return `<div class="code-block"><div class="code-block__lang">${md.utils.escapeHtml(lang)}</div>${body}</div>`
}

/** 自定义 image 渲染：alt 文字作为图片下方说明 */
const rawImage = md.renderer.rules.image!
md.renderer.rules.image = (tokens, idx, options, env, self): string => {
  const token = tokens[idx]!
  const src = token.attrGet('src') ?? ''
  const alt = token.content
  const img = rawImage(tokens, idx, options, env, self)
  if (!alt) return img
  const escapedAlt = md.utils.escapeHtml(alt)
  return `<figure><img src="${md.utils.escapeHtml(src)}" alt="${escapedAlt}" loading="lazy"><figcaption>${escapedAlt}</figcaption></figure>`
}

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

/** DOM 更新后执行代码高亮 */
watch(renderedContent, async () => {
  await nextTick()
  hljs.highlightAll()
})
</script>

<template>
  <main class="blog-detail">
    <p v-if="loading" class="blog-detail__status">加载中...</p>
    <p v-else-if="!blog" class="blog-detail__status">博客不存在</p>
    <div v-else class="blog-detail__layout">
      <!-- 左侧：元信息 -->
      <aside class="blog-detail__side glass">
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
      <article v-if="blog.content" class="blog-detail__content glass" v-html="renderedContent"></article>

      <!-- 右侧留空 -->
      <div class="blog-detail__right"></div>
    </div>
  </main>
</template>

<style scoped>
.blog-detail {
  padding: 32px 24px 64px;
}

.blog-detail__status {
  text-align: center;
  padding: 64px 0;
  font-size: 1rem;
  color: var(--color-text-secondary);
}

.blog-detail__layout {
  max-width: 1400px;
  margin: 0 auto;
  display: grid;
  grid-template-columns: 1fr 3fr 1fr;
  gap: 40px;
}

/* ── 左侧 ── */

.blog-detail__side {
  display: flex;
  flex-direction: column;
  gap: 16px;
  position: sticky;
  top: 120px;
  align-self: start;
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
@import "@/assets/glass.css";
@import "@/assets/markdown.css";
</style>
