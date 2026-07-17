<script setup lang="ts">
</script>

<template>
  <nav class="navbar">
    <RouterLink to="/" class="navbar-logo">石木古月</RouterLink>
  </nav>
</template>

<style scoped>
.navbar {
  display: flex;
  align-items: center;
  height: 80px;
  padding: 0 32px;
  background-color: #fff;
  border-bottom: 1px solid #e8e8e8;
  box-shadow: 0 1px 4px rgba(0, 0, 0, 0.06);
}

.navbar-logo {
  font-size: 1.8rem;
  font-weight: 700;
  color: #333;
  text-decoration: none;
}
</style>
