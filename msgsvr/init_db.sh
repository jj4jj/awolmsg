#!/bin/bash


MYSQL="mysql -ugsgame -pgsgame "
$MYSQL -Bse "create database IF NOT EXISTS msg;"
$MYSQL -Bse "use msg;create table if not exists msg (
    type SMALLINT UNSIGNED   NOT NULL,
    actor VARCHAR(192) NOT NULL,
    msgid BIGINT UNSIGNED NOT NULL,
    msg MEDIUMBLOB,
    PRIMARY KEY(type,actor,msgid)
) ENGINE=InnoDB
DEFAULT CHARACTER SET=utf8 COLLATE=utf8_general_ci;"

