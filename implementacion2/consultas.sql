--get train data
--para cada grupo r in ([0,1] - [parte/k])
SELECT count(*) 
FROM 
(SELECT user_id, item_id, rating 
    FROM ml100k 
    WHERE rand >= 0 AND rAND < 0.8 
    AND rating > 0) A 
WHERE user_id IN 
(SELECT user_id 
    FROM ml100k 
    GROUP BY user_id 
    HAVING count(*) > 2);


DROP TABLE IF EXISTS tempA;
DROP TABLE IF EXISTS tempB;

CREATE TABLE tempA(user_id int, item_id int, rating int);
CREATE TABLE tempB(user_id int, item_id int, rating int);

DELETE FROM tempA;
DELETE FROM tempB;


INSERT 
SELECT * FROM 
(SELECT user_id, item_id, rating 
    FROM ml100k 
    WHERE rand >= 0 AND rAND < 0.8 
    AND rating > 0) A 
WHERE user_id IN 
(SELECT user_id 
    FROM ml100k 
    GROUP BY user_id 
    HAVING count(*) > 2);


--temp1 = {u,i,r}

SELECT user_id FROM 
(SELECT user_id, item_id, rating 
    FROM ml100k 
    WHERE rand >= 0 AND rAND < 0.8 
    AND rating > 0) A 
WHERE user_id IN 
(SELECT user_id 
    FROM ml100k 
    GROUP BY user_id 
    HAVING count(*) > 2);

INSERT SELECT * FROM (SELECT user_id, item_id, rating FROM ml100k WHERE rand >= 0 AND rAND < 0.8 AND rating > 0) A WHERE user_id IN (SELECT user_id FROM ml100k GROUP BY user_id HAVING count(*) > 2);

INSERT INTO test SELECT * FROM (SELECT user_id, item_id FROM ml100k WHERE rand > 0.8 AND rand <= 1 AND rating = 5) A WHERE user_id IN (SELECT user_id FROM tempa GROUP BY user_id) AND item_id IN (SELECT item_id FROM tempa GROUP BY item_id);


--Proceso para eliminar instancias repetidas que cuenten con el mismo par user_id, item_id
--Este proceso debe aplicarse en todos los dataset que tengan instancias repetidas.
--Se puede evaluar la existencia de instancias repetidas en cada dataset con la siguiente consulta:

select sum(count), count(*) 
from (
    select user_id, item_id, count(*) 
    from anime_index 
    group by user_id, item_id 
    having count(*) > 1
) A

--Los datos repetidos no están contemplados en la formulación del problema 
--por lo que la implementación actual puede ocacionar confiltos al encontrarse con estos
--Se han encontrado instancias repetidas en los siguientes datasets:
--Anime
--Amazon
--GoodAmazon


--Si bien es un efecto que se puede evitar desde la recopilación de datos, es importante evitar su incidencia en la inserción de métodos de filtrado colaborativo

select A.rating
from dataset A inner join dataset B 
on A.user_id = B.user_id
and A.item_id = B.item_id
and A.rating > B.rating


select user_id, item_id, count(*) 
from anime_index 
group by user_id, item_id 
having count(*) > 1

create table tempc(user_id int, item_id int, rating int, time int, rand decimal(4,3));
--Identifica los elementos repetidos a conservar

delete from tempc;

insert into tempc(
    select user_id, item_id, rating, time, rand
    from (
        select *, row_number() over(
            partition by user_id, item_id
            order by user_id, item_id
        ) N
        from good_amazon_index
        where concat(user_id, ' ', item_id) in (
            select concat(user_id, ' ', item_id) 
            from good_amazon_index 
            group by user_id, item_id 
            having count(*) > 1
        ) 
    ) A
    where n = 2
)

--Elimina todos los elmentos repetidos
delete from good_amazon_index 
where concat(user_id, ' ', item_id)
in (
    select concat(user_id, ' ', item_id)
    from good_amazon_index
    group by user_id, item_id
    having count(*) > 1
)

--Ïmsertar los elementos a conservar
insert into good_amazon_index(
    select * 
    from tempc
)



--Identifica los elementos repetidos a conservar
delete from tempc;

insert into tempc(
    select user_id, item_id, rating, time
    from (
        select *, row_number() over(
            partition by user_id, item_id
            order by user_id, item_id
        ) N
        from amazon_index
        where concat(user_id, ' ', item_id) in (
            select concat(user_id, ' ', item_id) 
            from amazon_index 
            group by user_id, item_id 
            having count(*) > 1
        ) 
    ) A
    where n = 2
);

--Elimina todos los elmentos repetidos
delete from amazon_index 
where concat(user_id, ' ', item_id)
in (
    select concat(user_id, ' ', item_id)
    from amazon_index
    group by user_id, item_id
    having count(*) > 1
);

--Ïmsertar los elementos a conservar
insert into amazon_index(
    select user_id, item_id, rating, time
    from tempc
);




--Identifica los elementos repetidos a conservar
delete from tempc;

insert into tempc(
    select user_id, item_id, rating, rating, rand
    from (
        select *, row_number() over(
            partition by user_id, item_id
            order by user_id, item_id
        ) N
        from anime_index
        where concat(user_id, ' ', item_id) in (
            select concat(user_id, ' ', item_id) 
            from anime_index 
            group by user_id, item_id 
            having count(*) > 1
        ) 
    ) A
    where n = 2
);

--Elimina todos los elmentos repetidos
delete from anime_index 
where concat(user_id, ' ', item_id)
in (
    select concat(user_id, ' ', item_id)
    from anime_index
    group by user_id, item_id
    having count(*) > 1
);

--Ïmsertar los elementos a conservar
insert into anime_index(
    select user_id, item_id, rating, rand
    from tempc
);

