<script setup lang="ts">
import { ref, computed } from 'vue'
import '../css/blog.css'

/* ========================================================================
   类型定义
   ======================================================================== */

interface Article {
  id: number
  title: string
  description: string
  category: string
  tags: string[]
  date: string
}

const articles = ref<Article[]>([])

/* ========================================================================
   筛选选项
   ======================================================================== */

const categories = computed<string[]>(() => {
  const set = new Set<string>()
  for (const a of articles.value) set.add(a.category)
  return [...set]
})

const allTags = computed<string[]>(() => {
  const set = new Set<string>()
  for (const a of articles.value) {
    for (const t of a.tags) set.add(t)
  }
  return [...set]
})

/* ========================================================================
   分类筛选状态
   ======================================================================== */

// 单选模式下当前选中的分类（空串 = 不限）
const selectedCategory = ref<string>('')
// 多选模式下选中的分类集合
const selectedCategories = ref<Set<string>>(new Set())
// 分类筛选模式：false = 单选，true = 多选
const categoryMulti = ref<boolean>(false)

/* ========================================================================
   标签筛选状态
   ======================================================================== */

const selectedTags = ref<Set<string>>(new Set())
const tagMulti = ref<boolean>(false)

/* ========================================================================
   搜索
   ======================================================================== */

const searchQuery = ref<string>('')

/* ========================================================================
   方法
   ======================================================================== */

/** 分类 chip 点击 */
function toggleCategory(cat: string) {
  if (!categoryMulti.value) {
    // 单选模式 — 再次点击取消选中
    selectedCategory.value = selectedCategory.value === cat ? '' : cat
  } else {
    // 多选模式
    const next = new Set(selectedCategories.value)
    if (next.has(cat)) {
      next.delete(cat)
    } else {
      next.add(cat)
    }
    selectedCategories.value = next
  }
  // 分类变化 → 重置标签
  selectedTags.value = new Set()
}

/** 标签 chip 点击 */
function toggleTag(tag: string) {
  if (!tagMulti.value) {
    // 单选模式 — 再次点击取消选中
    if (selectedTags.value.has(tag)) {
      selectedTags.value = new Set()
    } else {
      selectedTags.value = new Set([tag])
    }
  } else {
    // 多选模式
    const next = new Set(selectedTags.value)
    if (next.has(tag)) {
      next.delete(tag)
    } else {
      next.add(tag)
    }
    selectedTags.value = next
  }
}

/** 切换分类筛选模式 */
function toggleCategoryMode() {
  categoryMulti.value = !categoryMulti.value
  // 模式切换时重置选中状态
  selectedCategory.value = ''
  selectedCategories.value = new Set()
  selectedTags.value = new Set()
}

/** 切换标签筛选模式 */
function toggleTagMode() {
  tagMulti.value = !tagMulti.value
  selectedTags.value = new Set()
}

/* ========================================================================
   计算 — 筛选后的文章列表
   ======================================================================== */

const filteredArticles = computed<Article[]>(() => {
  let result = articles.value

  // --- 分类过滤 ---
  if (categoryMulti.value) {
    // 多选模式：选中为空 = 不限
    const cats = selectedCategories.value
    if (cats.size > 0) {
      result = result.filter((a) => cats.has(a.category))
    }
  } else {
    // 单选模式：空串 = 不限
    if (selectedCategory.value !== '') {
      result = result.filter((a) => a.category === selectedCategory.value)
    }
  }

  // --- 标签过滤 (OR 逻辑) ---
  if (selectedTags.value.size > 0) {
    const tags = selectedTags.value
    result = result.filter((a) => a.tags.some((t) => tags.has(t)))
  }

  // --- 搜索过滤 ---
  const q = searchQuery.value.trim().toLowerCase()
  if (q) {
    result = result.filter(
      (a) =>
        a.title.toLowerCase().includes(q) ||
        a.description.toLowerCase().includes(q),
    )
  }

  return result
})

/* ========================================================================
   计算 — 当前分类下的可选标签
   ======================================================================== */

const availableTags = computed<string[]>(() => {
  // 先用分类筛选出子集
  let subset = articles.value
  if (categoryMulti.value) {
    const cats = selectedCategories.value
    if (cats.size > 0) {
      subset = subset.filter((a) => cats.has(a.category))
    }
  } else {
    if (selectedCategory.value !== '') {
      subset = subset.filter((a) => a.category === selectedCategory.value)
    }
  }
  // 收集该子集中所有标签
  const tagSet = new Set<string>()
  for (const a of subset) {
    for (const t of a.tags) {
      tagSet.add(t)
    }
  }
  // 保持 allTags 原有顺序
  return allTags.value.filter((t) => tagSet.has(t))
})
</script>

<template>
  <main class="blog">
    <!-- ============================================================
    筛选器
    ============================================================ -->
    <section class="blog__filter">
      <!-- 第一行：分类 -->
      <div class="blog__filter-row">
        <span class="blog__filter-label">分类</span>
        <button class="blog__mode-btn" @click="toggleCategoryMode">
          {{ categoryMulti ? '多选' : '单选' }}
        </button>
        <div class="blog__filter-chips">
          <button
            v-for="cat in categories"
            :key="cat"
            class="blog__chip"
            :class="{
              'blog__chip--active': categoryMulti
                ? selectedCategories.has(cat)
                : selectedCategory === cat,
            }"
            @click="toggleCategory(cat)"
          >
            {{ cat }}
          </button>
        </div>
      </div>

      <!-- 第二行：标签 -->
      <div class="blog__filter-row">
        <span class="blog__filter-label">标签</span>
        <button class="blog__mode-btn" @click="toggleTagMode">
          {{ tagMulti ? '多选' : '单选' }}
        </button>
        <div class="blog__filter-chips">
          <button
            v-for="tag in availableTags"
            :key="tag"
            class="blog__chip"
            :class="{
              'blog__chip--active': selectedTags.has(tag),
            }"
            @click="toggleTag(tag)"
          >
            {{ tag }}
          </button>
        </div>
      </div>

      <!-- 搜索框 -->
      <div class="blog__search">
        <input
          v-model="searchQuery"
          class="blog__search-input"
          type="text"
          placeholder="搜索文章标题或描述…"
        />
      </div>
    </section>

    <!-- ============================================================
    卡片网格
    ============================================================ -->
    <section class="blog__grid">
      <template v-if="filteredArticles.length > 0">
        <RouterLink
          v-for="article in filteredArticles"
          :key="article.id"
          :to="`/blogs/${article.id}`"
          class="blog__card"
        >
          <h2 class="blog__card-title">{{ article.title }}</h2>
          <p class="blog__card-desc">{{ article.description }}</p>
          <div class="blog__card-meta">
            <!-- 分类 -->
            <span class="blog__card-category">{{ article.category }}</span>
            <!-- 标签 -->
            <span
              v-for="tag in article.tags"
              :key="tag"
              class="blog__card-tag"
            >
              {{ tag }}
            </span>
          </div>
          <time class="blog__card-time" :datetime="article.date">
            {{ article.date }}
          </time>
        </RouterLink>
      </template>
      <p v-else class="blog__empty">没有找到匹配的文章</p>
    </section>
  </main>
</template>
