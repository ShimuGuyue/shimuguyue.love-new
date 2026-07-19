<script setup lang="ts">
import { ref, watch, computed, onMounted } from 'vue'
import { useRouter } from 'vue-router'

const router = useRouter()

// ── 类型定义 ──

interface Category {
  id: number
  name: string
}

interface Tag {
  id: number
  name: string
  category_id: number
}

interface BlogItem {
  id: number
  title: string
  description: string | null
  update_time: string
  category: string | null
  file_path: string | null
  tags: string[]
}

// ── 数据 ──

const categories = ref<Category[]>([])
const tags = ref<Tag[]>([])
const blogs = ref<BlogItem[]>([])

// ── 筛选状态 ──

const selectedCategoryIds = ref<number[]>([])
const categoryMulti = ref(false)

const selectedTagIds = ref<number[]>([])
const tagMulti = ref(false)

const searchQuery = ref('')

let searchTimer: ReturnType<typeof setTimeout> | null = null

// ── 计算：当前可见的标签（按选中的分类过滤） ──

const visibleTags = computed<Tag[]>(() => {
  if (selectedCategoryIds.value.length === 0) {
    // 未选分类时显示所有标签
    return tags.value
  }
  // 多选时取所有选中分类的标签并集
  const idSet = new Set(selectedCategoryIds.value)
  return tags.value.filter(t => idSet.has(t.category_id))
})

// 分类切换后清除已不存在的标签选中
watch(selectedCategoryIds, () => {
  const visibleIds = new Set(visibleTags.value.map(t => t.id))
  selectedTagIds.value = selectedTagIds.value.filter(id => visibleIds.has(id))
})

// ── 远程获取 ──

const loading = ref(true)

async function fetchCategories() {
  try {
    const resp = await fetch('/api/categories')
    if (!resp.ok) throw new Error(`HTTP ${resp.status}`)
    categories.value = await resp.json()
  } catch (e) { console.error('获取分类失败:', e) }
}

async function fetchTags() {
  try {
    const resp = await fetch('/api/tags')
    if (!resp.ok) throw new Error(`HTTP ${resp.status}`)
    tags.value = await resp.json()
  } catch (e) { console.error('获取标签失败:', e) }
}

async function fetchBlogs() {
  const params = new URLSearchParams()

  if (selectedCategoryIds.value.length > 0) {
    params.set('category_ids', selectedCategoryIds.value.join(','))
  }

  if (selectedTagIds.value.length > 0) {
    params.set('tag_ids', selectedTagIds.value.join(','))
  }

  const q = searchQuery.value.trim()
  if (q) {
    params.set('q', q)
  }

  loading.value = true
  try {
    const resp = await fetch('/api/blogs?' + params.toString())
    if (!resp.ok) throw new Error(`HTTP ${resp.status}`)
    blogs.value = await resp.json()
  } catch (e) {
    console.error('获取博客失败:', e)
    blogs.value = []
  } finally {
    loading.value = false
  }
}

// ── 筛选操作 ──

function toggleCategory(id: number) {
  if (categoryMulti.value) {
    const idx = selectedCategoryIds.value.indexOf(id)
    if (idx >= 0) {
      selectedCategoryIds.value.splice(idx, 1)
    } else {
      selectedCategoryIds.value.push(id)
    }
  } else {
    selectedCategoryIds.value =
      selectedCategoryIds.value[0] === id ? [] : [id]
  }
  fetchBlogs()
}

function toggleTag(id: number) {
  if (tagMulti.value) {
    const idx = selectedTagIds.value.indexOf(id)
    if (idx >= 0) {
      selectedTagIds.value.splice(idx, 1)
    } else {
      selectedTagIds.value.push(id)
    }
  } else {
    selectedTagIds.value =
      selectedTagIds.value[0] === id ? [] : [id]
  }
  fetchBlogs()
}

function onSearchInput() {
  if (searchTimer) clearTimeout(searchTimer)
  searchTimer = setTimeout(fetchBlogs, 300)
}

function goToBlog(filePath: string) {
  router.push(`/blogs/${filePath.replace(/^\/+/, "")}`)
}

// ── 生命周期 ──

onMounted(async () => {
  await Promise.all([fetchCategories(), fetchTags()])
  await fetchBlogs()
})
</script>

<template>
  <main class="blogs-page">
    <!-- ── 筛选器 ── -->
    <section class="filter-bar">
      <!-- 分类筛选 -->
      <div class="filter-row">
        <span class="filter-label">分类</span>
        <button class="filter-mode-btn" @click="categoryMulti = !categoryMulti">{{ categoryMulti ? '多选' : '单选' }}</button>
        <div class="filter-chips">
          <button
            v-for="cat in categories"
            :key="cat.id"
            class="chip"
            :class="{ 'chip--active': selectedCategoryIds.includes(cat.id) }"
            @click="toggleCategory(cat.id)"
          >
            {{ cat.name }}
          </button>
        </div>
      </div>

      <!-- 标签筛选 -->
      <div class="filter-row">
        <span class="filter-label">标签</span>
        <button class="filter-mode-btn" @click="tagMulti = !tagMulti">{{ tagMulti ? '多选' : '单选' }}</button>
        <div class="filter-chips">
          <button
            v-for="tag in visibleTags"
            :key="tag.id"
            class="chip"
            :class="{ 'chip--active': selectedTagIds.includes(tag.id) }"
            @click="toggleTag(tag.id)"
          >
            {{ tag.name }}
          </button>
        </div>
      </div>

      <!-- 搜索 -->
      <div class="filter-search">
        <input
          v-model="searchQuery"
          type="text"
          class="search-input"
          placeholder="搜索标题、描述、分类和标签中的内容..."
          @input="onSearchInput"
        />
      </div>
    </section>

    <!-- ── 博客卡片网格 ── -->
    <p v-if="loading" class="blog-status">加载中...</p>
    <p v-else-if="!blogs.length" class="blog-status">未检索到对应博客</p>
    <section v-else class="blog-grid">
      <article
        v-for="blog in blogs"
        :key="blog.id"
        class="blog-card"
        @click="goToBlog(blog.file_path || '')"
      >
        <h3 class="blog-card__title">{{ blog.title }}</h3>
        <p class="blog-card__desc">{{ blog.description }}</p>
        <div class="blog-card__meta blog-tags">
          <span v-if="blog.category" class="blog-category">{{ blog.category }}</span>
          <span
            v-for="tag in blog.tags"
            :key="tag"
            class="blog-tag"
          >{{ tag }}</span>
        </div>
        <time class="blog-card__time">{{ blog.update_time }}</time>
      </article>
    </section>
  </main>
</template>

<style scoped>
/* ── 页面 ── */

.blogs-page {
  max-width: 1200px;
  margin: 0 auto;
  padding: 24px 32px 48px;
}

/* ── 筛选器 ── */

.filter-bar {
  background-color: var(--color-card-bg, var(--color-nav-bg));
  border: 1px solid var(--color-border);
  border-radius: 8px;
  padding: 20px 24px;
  margin-bottom: 28px;
}

.filter-row {
  display: flex;
  align-items: flex-start;
  gap: 12px;
  margin-bottom: 16px;
}

.filter-label {
  flex-shrink: 0;
  width: 48px;
  padding-top: 6px;
  font-size: 1.1rem;
  font-weight: 600;
  color: var(--color-text);
  text-align: right;
  transform: translateY(-7px);
}

.filter-chips {
  flex: 1;
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
}

.chip {
  padding: 4px 14px;
  font-size: 0.85rem;
  color: var(--color-text-secondary);
  background: transparent;
  border: 1px solid var(--color-border);
  border-radius: 14px;
  cursor: pointer;
  transition: all 0.2s;
}

.chip:hover {
  border-color: var(--color-text-secondary);
  color: var(--color-text);
}

.chip--active {
  color: #fff;
  background-color: var(--color-text);
  border-color: var(--color-text);
}

.filter-mode-btn {
  flex-shrink: 0;
  margin-top: 2px;
  padding: 2px 10px;
  font-size: 0.75rem;
  color: var(--color-text-secondary);
  background: transparent;
  border: 1px solid var(--color-border);
  border-radius: 4px;
  cursor: pointer;
  transition: all 0.2s;
}

.filter-mode-btn:hover {
  color: var(--color-text);
  border-color: var(--color-text-secondary);
}

.filter-search {
  margin-top: 4px;
}

.search-input {
  width: 100%;
  box-sizing: border-box;
  padding: 8px 14px;
  font-size: 0.9rem;
  color: var(--color-text);
  background: transparent;
  border: 1px solid var(--color-border);
  border-radius: 4px;
  outline: none;
  transition: border-color 0.2s;
}

.search-input::placeholder {
  color: var(--color-text-secondary);
  opacity: 0.5;
}

.search-input:focus {
  border-color: var(--color-text-secondary);
}

/* ── 状态提示 ── */

.blog-status {
  text-align: center;
  padding: 48px 0;
  font-size: 0.95rem;
  color: var(--color-text-secondary);
}

/* ── 博客网格 ── */

.blog-grid {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 20px;
}


</style>

<style>
@import "@/assets/blog.css";
</style>
