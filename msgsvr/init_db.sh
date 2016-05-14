#!/bin/bash


MYSQL="mysql -utest -p123456 "
$MYSQL -Bse "create database IF NOT EXISTS test;"
$MYSQL -Bse "create table if not exists test.msg (
    type SMALLINT UNSIGNED   NOT NULL,
    actor VARCHAR(192) NOT NULL,
    msgid BIGINT UNSIGNED NOT NULL,
    msg MEDIUMBLOB,
    version INT(10) NOT NULL DEFAULT 0,
    PRIMARY KEY(type,actor,msgid)
) ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci;"

