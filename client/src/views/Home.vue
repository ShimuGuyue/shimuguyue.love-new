<script setup lang="ts">
</script>

<template>
  <main class="home">
    <div class="home__layout">
      <div class="home__photo glass"></div>
      <div class="home__info"></div>
    </div>
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
  min-height: 100%;
}

.home__info {
  /* 右侧信息栏 */
}
</style>

<style>
@import "@/assets/glass.css";
@import "@/assets/pink-theme.css";
</style>
