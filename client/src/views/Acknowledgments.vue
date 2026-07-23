<script setup lang="ts">
</script>

<template>
  <main class="thanks">
    <div class="thanks__card glass">
      <h1>致谢</h1>
      <p>
        照片墙标签背景图片来自第三方资源：
        <a href="https://www.flaticon.com/free-stickers/sticky-note" 
           title="sticky note stickers" 
           target="_blank" 
           rel="noopener noreferrer">
          Sticky note stickers created by jocularityart - Flaticon
        </a>
      </p>
      <p>
        网站图标图片来自第三方资源：
        <a href="https://www.flaticon.com/free-icons/study" 
           title="study icons" 
           target="_blank" 
           rel="noopener noreferrer">
          Study icons created by Magnific - Flaticon
        </a>
      </p>
    </div>
  </main>
</template>

<style scoped>
.thanks {
  padding: 48px 24px;
  display: flex;
  justify-content: center;
}

.thanks__card {
  padding: 48px;
  max-width: 1000px;
  width: 100%;
  text-align: center;
  border-radius: 8px;
}

.thanks__card h1 {
  margin: 0 0 24px;
  font-size: 1.6rem;
  color: var(--color-text);
}

.thanks__card p {
  margin: 0;
  font-size: 1rem;
  color: var(--color-text-secondary);
  line-height: 1.8;
}
</style>

<style>
@import "@/assets/glass.css";
@import "@/assets/pink-theme.css";
</style>
