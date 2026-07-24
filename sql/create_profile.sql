-- ============================================================
-- 主页个人介绍表（单行数据，仅修改不新增）
-- ============================================================
-- id:        固定为 1
-- title:     标题
-- subtitle:  副标题
-- bio:       简介
-- ============================================================

CREATE TABLE profile (
    id       SERIAL PRIMARY KEY,
    title    TEXT NOT NULL DEFAULT '标题',
    subtitle TEXT NOT NULL DEFAULT '副标题',
    bio      TEXT NOT NULL DEFAULT '简介'
);

COMMENT ON TABLE  profile IS '主页个人介绍表';
COMMENT ON COLUMN profile.id       IS '固定为 1';
COMMENT ON COLUMN profile.title    IS '标题';
COMMENT ON COLUMN profile.subtitle IS '副标题';
COMMENT ON COLUMN profile.bio      IS '简介';

-- 自动插入默认行
INSERT INTO profile (title, subtitle, bio) VALUES ('标题', '副标题', '简介');
