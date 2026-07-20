<script setup lang="ts">
import { ref, computed, onMounted, nextTick, watch } from 'vue'
import { useRoute } from 'vue-router'
import MarkdownIt from 'markdown-it'
import taskLists from 'markdown-it-task-lists'
import hljs from 'highlight.js'
import 'highlight.js/styles/github.css'
import katex from 'katex'
import 'katex/dist/katex.min.css'

const route = useRoute()

const md = new MarkdownIt({
  html: true,
  linkify: true,
  typographer: true,
}).use(taskLists, { enabled: true, label: true, labelAfter: true })

md.enable('strikethrough')

/** 自定义 fence 渲染：在代码块上显示语言标签 */
const rawFence = md.renderer.rules.fence!
md.renderer.rules.fence = (tokens, idx, options, env, self): string => {
  const token = tokens[idx]!
  const lang = token.info.trim().split(/\s+/)[0] ?? ''
  const body = rawFence(tokens, idx, options, env, self)
  if (!lang) return body
  return `<div class="code-block"><div class="code-block__lang">${md.utils.escapeHtml(lang)}</div>${body}</div>`
}

/** 自定义 heading_open：添加锚点 id 和 # 链接 */
const fallbackOpen = (tokens: any, idx: any, options: any, env: any, self: any) => self.renderToken(tokens, idx, options)
const rawHeadingOpen = md.renderer.rules.heading_open || fallbackOpen
md.renderer.rules.heading_open = (tokens, idx, options, env, self): string => {
  const token = tokens[idx]!
  const nextToken = tokens[idx + 1]
  let slug = ''
  if (nextToken && nextToken.type === 'inline') {
    slug = nextToken.content.replace(/[^a-zA-Z0-9\u4e00-\u9fff]+/g, '-').replace(/^-|-$/g, '').toLowerCase()
    token.attrSet('id', slug)
  }
  const html = rawHeadingOpen(tokens, idx, options, env, self)
  return slug ? html + '<a class="heading-anchor" href="#' + slug + '">#</a>' : html
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
  let text = blog.value.content
  // KaTeX 预处理：$$...$$ 和 $...$
  text = text.replace(/\$\$([^$]+)\$\$/g, (_, f) => renderKatex(f, true))
  text = text.replace(/\$([^$]+)\$/g, (_, f) => renderKatex(f, false))
  return md.render(text)
})

function renderKatex(formula: string, display: boolean): string {
  try {
    return katex.renderToString(formula, { displayMode: display, throwOnError: false })
  } catch {
    return display ? `$${formula}$` : `$${formula}$$`
  }
}

interface FlatTocItem {
  level: number
  text: string
  slug: string
}

const headings = computed<FlatTocItem[]>(() => {
  if (!blog.value?.content) return []
  const flat: FlatTocItem[] = []
  const re = /^(#{1,6})\s+(.+)$/gm
  let m: RegExpExecArray | null
  while ((m = re.exec(blog.value.content)) !== null) {
    const text = m[2]!.trim()
    const slug = text.replace(/[^a-zA-Z0-9\u4e00-\u9fff]+/g, '-').replace(/^-|-$/g, '').toLowerCase()
    const level = m[1]!.length
    if (level <= 4) flat.push({ level, text, slug })
  }
  return flat
})

function scrollToHeading(slug: string) {
  document.getElementById(slug)?.scrollIntoView({ behavior: 'smooth', block: 'start' })
}

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

      <!-- 右侧：目录 -->
      <nav v-if="headings.length" class="blog-detail__toc">
        <h4 class="toc-title">目录</h4>
        <ul class="toc-list">
          <li
            v-for="h in headings"
            :key="h.slug"
            class="toc-item"
            :style="{ paddingLeft: `${(h.level - 1) * 1}em` }"
            @click.stop="scrollToHeading(h.slug)"
          >
            <span class="toc-link">{{ h.text }}</span>
          </li>
        </ul>
      </nav>
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

/* ── 右侧目录 ── */
.blog-detail__toc {
  position: sticky;
  top: 100px;
  align-self: start;
  max-height: calc(100vh - 120px);
  overflow-y: auto;
}

.toc-title {
  margin: 0 0 12px;
  font-size: 0.95rem;
  font-weight: 600;
  color: var(--color-text);
}

.toc-list {
  list-style: none;
  padding: 0 !important;
  margin: 0 !important;
}

.toc-item {
  margin: 0 !important;
  padding-top: 4px;
  padding-bottom: 4px;
  font-size: 0.95rem;
  line-height: 1.4;
  color: var(--color-text-secondary);
  cursor: pointer;
}

.toc-link {
  position: relative;
  padding-left: 12px;
  display: inline-block;
}

/* 指示线严格只相对于文字单行高度定位 */
.toc-link::before {
  content: '';
  position: absolute;
  left: 0;
  top: 2px;
  bottom: 2px;
  border-left: 2px solid transparent;
  transition: border-color var(--transition-speed);
}

.toc-item:hover .toc-link {
  color: var(--pink-hot, #FF77CC);
}

.toc-item:hover .toc-link::before {
  border-left-color: var(--pink-hot, #FF77CC);
}
</style>

<style>
@import "@/assets/blog.css";
@import "@/assets/glass.css";
@import "@/assets/markdown.css";
@import "katex/dist/katex.min.css";
</style>
