// 正确输入
create table user (uid int,amount float);

insert into user (uid, amount) values (132,43);
insert into user values (132,43);

select uid,amount from user, banks where user<banks;

delete from user;
delete from user where uid<fd;

drop table user;