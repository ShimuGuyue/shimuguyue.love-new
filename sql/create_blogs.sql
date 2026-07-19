-- ============================================================
-- 类别（分类）表
-- ============================================================
-- id:   自增主键
-- name: 类别名
-- ============================================================

CREATE TABLE categories (
    id   SERIAL       PRIMARY KEY,
    name TEXT NOT NULL UNIQUE
);

COMMENT ON TABLE  categories IS '类别（分类）表';
COMMENT ON COLUMN categories.id   IS '主键，自增';
COMMENT ON COLUMN categories.name IS '类别名';

-- ============================================================
-- 博客表
-- ============================================================
-- id:           自增主键
-- title:        博客标题
-- description:  简介 / 描述
-- updated_at:   更新时间（精确到日）
-- content:      博客正文内容
-- file_path:    关联文件相对路径
-- category_id:  所属分类（一对多：一个分类下有多个博客）
-- ============================================================

CREATE TABLE blogs (
    id          SERIAL  PRIMARY KEY,
    title       TEXT    NOT NULL UNIQUE,
    description TEXT    NOT NULL,
    updated_at  DATE    NOT NULL DEFAULT CURRENT_DATE,
    content     TEXT    NOT NULL,
    file_path   TEXT    NOT NULL UNIQUE,
    category_id INT     REFERENCES categories(id) ON DELETE SET NULL
);

COMMENT ON TABLE  blogs IS '博客表';
COMMENT ON COLUMN blogs.id           IS '主键，自增';
COMMENT ON COLUMN blogs.title        IS '博客标题';
COMMENT ON COLUMN blogs.description  IS '简介 / 描述';
COMMENT ON COLUMN blogs.updated_at   IS '更新时间（精确到日）';
COMMENT ON COLUMN blogs.content      IS '博客正文';
COMMENT ON COLUMN blogs.file_path    IS '关联文件相对路径';
COMMENT ON COLUMN blogs.category_id  IS '所属分类';

-- 按分类查询其下所有博客
CREATE INDEX idx_blogs_category
    ON blogs (category_id);

-- ============================================================
-- 标签表
-- ============================================================
-- id:          自增主键
-- name:        标签名
-- category_id: 所属分类（一对多：一个分类下有多个标签）
-- ============================================================

CREATE TABLE tags (
    id          SERIAL  PRIMARY KEY,
    name        TEXT    NOT NULL UNIQUE,
    category_id INT     NOT NULL REFERENCES categories(id) ON DELETE CASCADE
);

COMMENT ON TABLE  tags IS '标签表';
COMMENT ON COLUMN tags.id           IS '主键，自增';
COMMENT ON COLUMN tags.name         IS '标签名';
COMMENT ON COLUMN tags.category_id  IS '所属分类';

-- 按分类查询其下所有标签
CREATE INDEX idx_tags_category
    ON tags (category_id);

-- ============================================================
-- 博客-标签关联表（多对多）
-- ============================================================
-- 查询场景：
--   博客 → 标签：WHERE blog_id = ?
--   标签 → 博客：WHERE tag_id  = ?
-- ============================================================

CREATE TABLE blog_tags (
    blog_id INT NOT NULL REFERENCES blogs(id) ON DELETE CASCADE,
    tag_id  INT NOT NULL REFERENCES tags(id)  ON DELETE CASCADE,
    PRIMARY KEY (blog_id, tag_id)
);

COMMENT ON TABLE  blog_tags IS '博客-标签关联表';
COMMENT ON COLUMN blog_tags.blog_id IS '关联 blogs.id';
COMMENT ON COLUMN blog_tags.tag_id  IS '关联 tags.id';

-- 按标签反查所属博客
CREATE INDEX idx_blog_tags_tag
    ON blog_tags (tag_id);
