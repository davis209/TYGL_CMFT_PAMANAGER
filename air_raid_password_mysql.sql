-- MySQL table for the single air-raid confirmation password.
CREATE TABLE IF NOT EXISTS PA_AIR_RAID_PASSWORD (
    PASSWORD_ID TINYINT UNSIGNED NOT NULL,
    PASSWORD_MD5 CHAR(32) NOT NULL,
    UPDATED_AT DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3),
    PRIMARY KEY (PASSWORD_ID),
    CONSTRAINT CK_PA_AIR_RAID_PASSWORD_ID CHECK (PASSWORD_ID = 1)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Initialise exactly one password before enabling this function. Replace the
-- placeholder with a controlled initial password, then execute this statement:
-- INSERT INTO PA_AIR_RAID_PASSWORD (PASSWORD_ID, PASSWORD_MD5, UPDATED_AT)
-- VALUES (1, MD5('<initial-password>'), NOW(3));
