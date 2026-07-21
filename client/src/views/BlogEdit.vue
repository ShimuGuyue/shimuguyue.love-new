<script setup lang="ts">
import { ref } from 'vue'

const title = ref('')
const description = ref('')
const category = ref('')
const tags = ref('')
const pathCategory = ref('')
const pathName = ref('')
</script>

<template>
  <main class="blog-edit">
    <div class="blog-edit__layout">
      <aside class="blog-edit__left">
        <div class="blog-edit__meta glass">
          <div class="blog-edit__field">
            <label class="blog-edit__label">标题</label>
            <input v-model="title" class="blog-edit__input" placeholder="博客标题" />
          </div>
          <div class="blog-edit__field">
            <label class="blog-edit__label">描述</label>
            <input v-model="description" class="blog-edit__input" placeholder="简短描述" />
          </div>
          <div class="blog-edit__field">
            <label class="blog-edit__label">分类</label>
            <input v-model="category" class="blog-edit__input" placeholder="分类名称" />
          </div>
          <div class="blog-edit__field">
            <label class="blog-edit__label">标签</label>
            <input v-model="tags" class="blog-edit__input" placeholder="用英文逗号分隔" />
          </div>
          <div class="blog-edit__field">
            <label class="blog-edit__label">文件路径</label>
            <div class="blog-edit__path-row">
              <input v-model="pathCategory" class="blog-edit__input blog-edit__path-input" placeholder="分类目录" />
              <span class="blog-edit__path-sep">/</span>
              <input v-model="pathName" class="blog-edit__input blog-edit__path-input" placeholder="文件名" />
            </div>
          </div>
        </div>
        <div class="blog-edit__actions">
          <button class="blog-edit__btn blog-edit__btn--secondary">导入文件</button>
          <button class="blog-edit__btn blog-edit__btn--primary">保存博客</button>
        </div>
      </aside>
      <section class="blog-edit__main glass">
        <div
          class="blog-edit__content"
          contenteditable="true"
          placeholder="在此编辑博客 Markdown 文本..."
        ></div>
      </section>
      <aside class="blog-edit__right"></aside>
    </div>
  </main>
</template>

<style scoped>
.blog-edit {
  padding: 32px 24px 64px;
}

.blog-edit__layout {
  max-width: 1400px;
  margin: 0 auto;
  display: grid;
  grid-template-columns: 1fr 2fr 1fr;
  gap: 40px;
}

/* ── 左侧 ── */
.blog-edit__left {
  display: flex;
  flex-direction: column;
}
.blog-edit__field {
  margin-bottom: 16px;
}
.blog-edit__label {
  display: block;
  margin-bottom: 4px;
  font-size: 0.85rem;
  color: var(--color-text-secondary);
}
.blog-edit__input {
  width: 100%;
  padding: 6px 10px;
  font-size: 0.9rem;
  color: var(--color-text);
  background: rgba(0, 0, 0, 0.04);
  border: 1px solid var(--color-border);
  border-radius: 4px;
  outline: none;
  transition: border-color var(--transition-speed);
}
.blog-edit__input:focus {
  border-color: var(--pink-soft);
}

.blog-edit__path-row {
  display: flex;
  align-items: center;
  gap: 6px;
}
.blog-edit__path-input {
  flex: 1;
  min-width: 0;
}
.blog-edit__path-sep {
  color: var(--color-text-secondary);
  font-size: 0.9rem;
}

.blog-edit__actions {
  display: flex;
  flex-direction: column;
  gap: 8px;
  margin-top: auto;
  padding-top: 16px;
}
.blog-edit__btn {
  padding: 8px 0;
  font-size: 0.85rem;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  transition: opacity var(--transition-speed);
}
.blog-edit__btn--primary {
  color: #fff;
  background-color: #FF77CC;
}
.blog-edit__btn--secondary {
  color: var(--color-text-secondary);
  background: rgba(0, 0, 0, 0.06);
  border: 1px solid var(--color-border);
}
.blog-edit__btn:hover {
  opacity: 0.85;
}

/* ── 中间 ── */
.blog-edit__content {
  width: 100%;
  height: calc(100vh - 226px);
  padding: 16px;
  font-family: "仿宋", FangSong, serif;
  font-size: 1.05rem;
  line-height: 1.8;
  color: var(--color-text);
  background: transparent;
  border: none;
  outline: none;
}
.blog-edit__content:empty::before {
  content: attr(placeholder);
  color: #999;
  pointer-events: none;
}
</style>

<style>
@import "@/assets/glass.css";
</style>
