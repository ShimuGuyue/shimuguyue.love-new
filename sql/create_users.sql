-- ============================================================
-- 创建 users 表
-- ============================================================
-- key_hash:     Argon2id 哈希字符串，用于个人身份认证（唯一）
-- username:     用户名，最多 10 个半角字符，不重复，可为空
-- password_hash: Argon2id 密码哈希，无用户名时可为空
-- key_enabled:  key 是否可用，默认启用
-- ============================================================

CREATE TABLE users (
    id            SERIAL       PRIMARY KEY,
    key_hash      TEXT         NOT NULL UNIQUE,
    key_enabled   BOOLEAN      NOT NULL DEFAULT TRUE,
    username      VARCHAR(10)  UNIQUE,
    password_hash TEXT
);

COMMENT ON TABLE  users IS '用户表';
COMMENT ON COLUMN users.id            IS '主键，自增';
COMMENT ON COLUMN users.key_hash      IS 'Argon2id 哈希，个人身份认证 token';
COMMENT ON COLUMN users.key_enabled   IS '认证 key 是否可用';
COMMENT ON COLUMN users.username      IS '用户名，最多 10 个半角字符';
COMMENT ON COLUMN users.password_hash IS 'Argon2id 密码哈希';
