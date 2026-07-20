declare module 'markdown-it-katex' {
  import type MarkdownIt from 'markdown-it'
  function mk(md: MarkdownIt, options?: { throwOnError?: boolean }): void
  export default mk
}
