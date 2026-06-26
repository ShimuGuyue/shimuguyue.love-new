<script setup lang="ts">
import { ref, computed } from 'vue'
import { useAuthStore } from '../stores/auth'
import '../css/blog.css'

/* ========================================================================
   类型
   ======================================================================== */

interface Frontmatter
{
  category: string
  tags: string[]
}

/* ========================================================================
   状态
   ======================================================================== */

const file = ref<File | null>(null)
const fileContent = ref('')
const editableBody = ref('')
const category = ref('')
const tags = ref('')
const message = ref('')

const auth = useAuthStore()

/* ========================================================================
   计算
   ======================================================================== */

const hasContent = computed(() => editableBody.value.length > 0)

const canFinish = computed(() => hasContent.value && auth.isSuperAdmin)

const tagList = computed<string[]>(() =>
{
  return tags.value
    .split(',')
    .map((s) => s.trim())
    .filter(Boolean)
})

/* ========================================================================
   方法
   ======================================================================== */

function onFileChange(e: Event)
{
  const input = e.target as HTMLInputElement
  if (!input.files || input.files.length === 0) return

  const f = input.files[0]
  file.value = f
  message.value = ''

  const reader = new FileReader()
  reader.onload = () =>
  {
    const raw = reader.result as string
    fileContent.value = raw
    parseFrontmatter(raw)
    const lines = raw.split('\n')
    if (lines[0]?.trim() === '---')
    {
      const end = lines.indexOf('---', 1)
      editableBody.value = end !== -1
        ? lines.slice(end + 1).join('\n').trim()
        : raw.trim()
    }
    else
    {
      editableBody.value = raw.trim()
    }
  }
  reader.readAsText(f)
}

function parseFrontmatter(md: string)
{
  const lines = md.split('\n')
  if (lines[0]?.trim() !== '---') return

  const end = lines.indexOf('---', 1)
  if (end === -1) return

  const fm: Record<string, string> = {}
  for (let i = 1; i < end; i++)
  {
    const line = lines[i]
    const colon = line.indexOf(':')
    if (colon === -1) continue
    const key = line.slice(0, colon).trim()
    const val = line.slice(colon + 1).trim()
    fm[key] = val
  }

  if (fm.category) category.value = fm.category
  if (fm.tags)
  {
    let raw = fm.tags
    if (raw.startsWith('[') && raw.endsWith(']'))
      raw = raw.slice(1, -1)
    tags.value = raw
      .split(',')
      .map((s) => s.trim())
      .filter(Boolean)
      .join(', ')
  }
}
</script>

<template>
  <main class="import">
    <!-- ============================================================
    顶部栏
    ============================================================ -->
    <div class="import__bar">
      <div class="import__bar-row">
        <div class="import__field">
          <label class="import__label" for="prop-type">类型</label>
          <input
            id="prop-type"
            v-model="category"
            class="import__input"
            type="text"
            placeholder="如：技术"
          />
        </div>

        <div class="import__field import__field--tags">
          <label class="import__label" for="prop-tags">标签</label>
          <input
            id="prop-tags"
            v-model="tags"
            class="import__input"
            type="text"
            placeholder="逗号分隔"
          />
        </div>

        <div class="import__field">
          <label class="import__label">&nbsp;</label>
          <label class="import__btn-doc" for="import-file-input">
            导入文档
          </label>
        </div>

        <div class="import__field">
          <label class="import__label">&nbsp;</label>
          <button
            class="import__btn-finish"
            :disabled="!canFinish"
          >
            {{ auth.isSuperAdmin ? '完成导入' : '无操作权限' }}
          </button>
        </div>
      </div>

      <div v-if="tagList.length > 0" class="import__tags">
        <span
          v-for="t in tagList"
          :key="t"
          class="import__tag-chip"
        >{{ t }}</span>
      </div>
    </div>

    <input
      id="import-file-input"
      class="import__file-input"
      type="file"
      accept=".md,.markdown"
      @change="onFileChange"
    />

    <!-- ============================================================
    毛玻璃编辑区
    ============================================================ -->
    <div class="import__canvas doc-bg">
      <div v-if="!hasContent" class="import__canvas-empty">
        <span class="import__drop-icon">↓</span>
        <span class="import__drop-text">点击「导入文档」或拖拽 Markdown 文件到此处</span>
      </div>
      <template v-else>
        <div class="import__rechoose">
          <label class="import__rechoose-link" for="import-file-input">重新选择</label>
        </div>
        <textarea
          v-model="editableBody"
          class="import__editor"
          placeholder="在此编辑 Markdown 正文…"
        ></textarea>
      </template>
    </div>

    <Transition name="import-toast">
      <p v-if="message" class="import__toast">{{ message }}</p>
    </Transition>
  </main>
</template>

<style scoped>
/* ========================================================================
   布局
   ======================================================================== */
.import
{
  max-width: 8rem;
  margin: 0 auto;
  height: 100vh;
  display: flex;
  flex-direction: column;
  padding: 0.96rem 0.16rem 0.16rem;
  box-sizing: border-box;
}

/* ========================================================================
   顶部栏
   ======================================================================== */
.import__bar
{
  flex-shrink: 0;
  background: var(--nav-bg);
  border: 1px solid var(--nav-border);
  border-radius: 0.08rem;
  padding: 0.16rem;
  margin-bottom: 0.16rem;
}

.import__bar-row
{
  display: flex;
  gap: 0.12rem;
  align-items: flex-end;
}

.import__field
{
  display: flex;
  flex-direction: column;
  gap: 0.04rem;
}

.import__field--tags
{
  flex: 1;
  min-width: 0;
}

.import__label
{
  font-size: 0.12rem;
  font-weight: 600;
  color: var(--nav-text);
  white-space: nowrap;
}

.import__input
{
  padding: 0.08rem 0.1rem;
  border: 1px solid var(--nav-border);
  border-radius: 0.04rem;
  font-size: 0.14rem;
  color: var(--text);
  background-color: var(--bg);
  outline: none;
  transition: border-color 0.2s;
  box-sizing: border-box;
  width: 100%;
}

.import__input:focus
{
  border-color: var(--nav-active);
}

/* ========================================================================
   导入文档按钮
   ======================================================================== */
.import__btn-doc
{
  display: inline-block;
  padding: 0.08rem 0.2rem;
  border: none;
  border-radius: 0.04rem;
  font-size: 0.14rem;
  font-weight: 600;
  background: linear-gradient(135deg, var(--nav-active), #6c5ce7);
  color: #fff;
  cursor: pointer;
  white-space: nowrap;
  transition: opacity 0.2s, transform 0.15s;
}

.import__btn-doc:hover
{
  transform: translateY(-1px);
}

.import__btn-doc:active
{
  transform: translateY(0);
}

/* ========================================================================
   完成导入按钮 — 与导入文档统一
   ======================================================================== */
.import__btn-finish
{
  display: inline-block;
  padding: 0.08rem 0.2rem;
  border: none;
  border-radius: 0.04rem;
  font-size: 0.14rem;
  font-weight: 600;
  background: linear-gradient(135deg, var(--nav-active), #6c5ce7);
  color: #fff;
  cursor: pointer;
  white-space: nowrap;
  transition: opacity 0.2s, transform 0.15s;
}

.import__btn-finish:hover:not(:disabled)
{
  transform: translateY(-1px);
}

.import__btn-finish:active:not(:disabled)
{
  transform: translateY(0);
}

.import__btn-finish:disabled
{
  opacity: 0.35;
  cursor: not-allowed;
}

/* ========================================================================
   标签预览
   ======================================================================== */
.import__tags
{
  display: flex;
  flex-wrap: wrap;
  gap: 0.06rem;
  margin-top: 0.1rem;
}

.import__tag-chip
{
  padding: 0.02rem 0.08rem;
  font-size: 0.11rem;
  color: var(--nav-active);
  background: rgba(55, 120, 229, 0.1);
  border-radius: 0.03rem;
}

[data-theme='dark'] .import__tag-chip
{
  background: rgba(93, 184, 216, 0.15);
}

.import__file-input
{
  position: absolute;
  width: 1px;
  height: 1px;
  overflow: hidden;
  clip: rect(0, 0, 0, 0);
}

/* ========================================================================
   编辑区
   ======================================================================== */
.import__canvas
{
  flex: 1;
  min-height: 0;
  display: flex;
  flex-direction: column;
  padding: 0.24rem;
}

/* ========================================================================
   空状态
   ======================================================================== */
.import__canvas-empty
{
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 0.12rem;
  color: var(--nav-text);
}

.import__drop-icon
{
  font-size: 0.48rem;
  color: var(--nav-active);
  opacity: 0.5;
}

.import__drop-text
{
  font-size: 0.15rem;
  text-align: center;
}

/* ========================================================================
   重新选择
   ======================================================================== */
.import__rechoose
{
  text-align: right;
  margin-bottom: 0.12rem;
  flex-shrink: 0;
}

.import__rechoose-link
{
  font-size: 0.12rem;
  color: var(--nav-active);
  cursor: pointer;
  text-decoration: underline;
}

/* ========================================================================
   Textarea
   ======================================================================== */
.import__editor
{
  flex: 1;
  min-height: 0;
  width: 100%;
  padding: 0;
  border: none;
  outline: none;
  resize: none;
  font-family: inherit;
  font-size: 0.14rem;
  line-height: 1.8;
  color: var(--text);
  background: transparent;
}

/* ========================================================================
   Toast
   ======================================================================== */
.import__toast
{
  position: fixed;
  bottom: 0.4rem;
  left: 50%;
  transform: translateX(-50%);
  z-index: 9999;
  padding: 0.1rem 0.24rem;
  font-size: 0.14rem;
  color: #fff;
  background: rgba(0, 0, 0, 0.78);
  border-radius: 0.06rem;
  backdrop-filter: blur(8px);
}

.import-toast-enter-active,
.import-toast-leave-active
{
  transition: all 0.25s ease;
}

.import-toast-enter-from,
.import-toast-leave-to
{
  opacity: 0;
  transform: translateX(-50%) translateY(0.12rem);
}
</style>