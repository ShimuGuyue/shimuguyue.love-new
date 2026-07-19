-- ============================================================
-- 创建 权限表
-- ============================================================
-- id:   自增主键
-- name: 权限名
-- ============================================================

CREATE TABLE permissions (
    id   SERIAL PRIMARY KEY,
    name TEXT   NOT NULL UNIQUE
);

COMMENT ON TABLE  permissions IS '权限表';
COMMENT ON COLUMN permissions.id   IS '主键，自增';
COMMENT ON COLUMN permissions.name IS '权限名';

-- ============================================================
-- 创建 用户-权限关联表
-- ============================================================
-- user_id:       关联 users.id
-- permission_id: 关联 permissions.id
-- 按 user_id 优先、permission_id 次之排序
-- ============================================================

CREATE TABLE user_permissions (
    user_id       INT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    permission_id INT NOT NULL REFERENCES permissions(id) ON DELETE CASCADE,
    PRIMARY KEY (user_id, permission_id)
);

COMMENT ON TABLE  user_permissions IS '用户-权限关联表';
COMMENT ON COLUMN user_permissions.user_id       IS '关联 users.id';
COMMENT ON COLUMN user_permissions.permission_id IS '关联 permissions.id';
