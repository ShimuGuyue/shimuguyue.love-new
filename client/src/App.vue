<script setup lang="ts">
import NavBar from '@/components/NavBar.vue'
</script>

<template>
  <NavBar />
  <RouterView />
</template>

<style>
*,
*::before,
*::after {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

:root {
  --color-nav-bg: #fff;
  --color-bg: #f5f5f5;
  --color-text: #333;
  --color-text-secondary: #666;
  --color-border: #e8e8e8;
  --color-hover: #f0f0f0;
}

html.dark {
  --color-nav-bg: #1e1e1e;
  --color-bg: #121212;
  --color-text: #e0e0e0;
  --color-text-secondary: #999;
  --color-border: #333;
  --color-hover: #2a2a2a;
}

body {
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto,
    'Helvetica Neue', Arial, sans-serif;
  background-color: var(--color-bg);
  color: var(--color-text);
  transition: background-color 0.3s, color 0.3s;
}
</style>
