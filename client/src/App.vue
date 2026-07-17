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

body {
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto,
    'Helvetica Neue', Arial, sans-serif;
  background-color: #f5f5f5;
  color: #333;
}
</style>
