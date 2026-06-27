<script setup lang="ts">
import { ref, onMounted, watch } from 'vue'
import { useRoute } from 'vue-router'
import MarkdownIt from 'markdown-it'
import githubAlerts from 'markdown-it-github-alerts'
import texmath from 'markdown-it-texmath'
import taskLists from 'markdown-it-task-lists'
import katex from 'katex'
import hljs from 'highlight.js'
import 'katex/dist/katex.min.css'
import 'highlight.js/styles/github.css'
import '../css/blog-detail.css'
import '../css/markdown.css'

const md = new MarkdownIt({
  html: true,
  linkify: true,
  typographer: true,
  highlight(str: string, lang: string): string {
    if (lang && hljs.getLanguage(lang)) {
      try {
        return hljs.highlight(str, { language: lang }).value
      } catch {
        /* fall through */
      }
    }
    return ''
  },
})

md.use(githubAlerts)
md.use(taskLists)
md.use(texmath, {
  engine: katex,
  delimiters: 'dollars',
  katexOptions: {
    throwOnError: false,
    displayMode: false,
  },
})

const route = useRoute()

/* ========================================================================
   状态
   ======================================================================== */

interface ArticleDetail {
  id: number
  title: string
  category: string
  tags: string[]
  time: string
  content: string
}

const article = ref<ArticleDetail | null>(null)
const html = ref('')
const loading = ref(true)
const error = ref('')

/* ========================================================================
   工具函数
   ======================================================================== */

/**
 * 剥离 YAML frontmatter（两行 --- 之间的内容）
 * 返回剥离后的正文；如果没有 frontmatter 则原样返回
 */
function stripFrontmatter(md: string): string {
  const trimmed = md.trimStart()
  if (!trimmed.startsWith('---')) return md
  const end = trimmed.indexOf('\n---', 3)
  if (end === -1) return md
  return trimmed.substring(end + 4).trimStart()
}

/* ========================================================================
   数据获取
   ======================================================================== */

let fetchSeq = 0

async function fetchArticle(title: string) {
  const seq = ++fetchSeq
  loading.value = true
  error.value = ''
  html.value = ''

  const url = `/api/blog/${encodeURIComponent(title)}`
  console.log(`[BlogDetail #${seq}] fetching`, url)

  try {
    const res = await fetch(url)
    if (!res.ok) {
      if (res.status === 404) throw new Error('文章不存在')
      throw new Error(`HTTP ${res.status}`)
    }
    const data = await res.json()
    console.log(`[BlogDetail #${seq}] response success=${data.success}, hasContent=${!!data.content}, contentLen=${data.content?.length ?? 0}`)

    if (!data.success) throw new Error(data.message ?? '未知错误')

    article.value = {
      id: data.id,
      title: data.title,
      category: data.category,
      tags: data.tags ?? [],
      content: data.content,
    }

    // 剥离 frontmatter 再渲染
    const body = stripFrontmatter(data.content ?? '')
    html.value = md.render(body)
    console.log(`[BlogDetail #${seq}] rendered html len=${html.value.length}`)
  } catch (e: unknown) {
    // 忽略过期请求
    if (seq !== fetchSeq) return
    const msg = e instanceof Error ? e.message : String(e)
    error.value = msg
    console.error('[BlogDetailPage]', msg)
  } finally {
    if (seq === fetchSeq) loading.value = false
  }
}

/* ========================================================================
   路由变化时重新取数
   ======================================================================== */

onMounted(() => {
  const title = route.params.title as string
  console.log('[BlogDetailPage] mounted, title=', title)
  if (title) fetchArticle(title)
})

watch(
  () => route.params.title,
  (newTitle) => {
    if (newTitle) {
      console.log('[BlogDetailPage] route changed, title=', newTitle)
      fetchArticle(newTitle as string)
    }
  },
)
</script>

<template>
  <article class="blog-detail">
    <!-- 加载中 -->
    <template v-if="loading">
      <div class="blog-detail__loading">正在加载文章…</div>
    </template>

    <!-- 出错 -->
    <template v-else-if="error">
      <div class="blog-detail__error">
        <h2>加载失败</h2>
        <p>{{ error }}</p>
      </div>
    </template>

    <!-- 文章内容 -->
    <template v-else-if="article">
      <div
        v-if="html"
        class="blog-detail__body markdown-body"
        v-html="html"
      />
      <p v-else class="blog-detail__empty-body">
        文章内容为空或文件尚未同步。
      </p>
    </template>
  </article>
</template>
