<script setup lang="ts">
</script>

<template>
  <main class="blog-edit">
    <div class="blog-edit__layout">
      <aside class="blog-edit__left"></aside>
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

.blog-edit__title {
  margin: 0 0 16px;
  font-size: 1.3rem;
  color: var(--color-text);
}

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
