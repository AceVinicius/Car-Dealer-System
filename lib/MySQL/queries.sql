-- Questao 1


-- Questao 2


-- Questão 3
SELECT SUM(valor_venda) - SUM(valor_custo) AS Lucro
FROM produto_pedido AS prod
INNER JOIN pedido AS ped
ON prod.cod_pedido = ped.cod
WHERE data_pedido >= '2000-02-02 00:00:00';


-- Questão 4


-- Questão 5
SELECT cod_agencia, SUM(salario) FROM car_dealer_system.funcionario AS f
LEFT JOIN car_dealer_system.setor AS s
ON f.cod_setor = s.cod
WHERE cod_agencia = 10
GROUP BY cod_agencia;


-- Questão 6
SELECT cod_setor, COUNT(cpf), SUM(salario) FROM funcionario AS f GROUP BY cod_setor;


-- Questão 7


-- Questão 8
SELECT cpf_cliente, SUM(valor_venda) 
FROM pedido AS ped
LEFT JOIN produto_pedido AS prod
ON ped.cod = prod.cod_pedido
GROUP BY cpf_cliente;


-- Questão 9
SELECT modelo, COUNT(modelo), SUM(valor_venda)
FROM carro AS c
INNER JOIN modelo AS m
ON c.cod_modelo = m.cod
GROUP BY modelo;


-- Questão 10
SELECT *
FROM pedido AS ped
LEFT JOIN produto_pedido AS prod
ON ped.cod = prod.cod_pedido
WHERE data_pedido < NOW() - INTERVAL 3 YEAR AND is_troca = 1;


-- Questão 11
SELECT cpf_funcionario
FROM (SELECT cpf_funcionario, SUM(valor_venda) - SUM(valor_custo) AS Lucro
		FROM (SELECT cod, cpf_funcionario
		FROM pedido AS p
		LEFT JOIN funcionario AS f
		ON p.cpf_funcionario = f.cpf
	) AS k
	LEFT JOIN produto_pedido AS prod
	ON prod.cod_pedido = k.cod
    WHERE is_troca = 0
	GROUP BY cpf_funcionario
	ORDER BY Lucro DESC
	LIMIT 1
) AS t;

SELECT cpf_funcionario
FROM (SELECT cpf_funcionario, SUM(valor_venda) - SUM(valor_custo) AS Lucro
		FROM (SELECT cod, cpf_funcionario
		FROM pedido AS p
		LEFT JOIN funcionario AS f
		ON p.cpf_funcionario = f.cpf
	) AS k
	LEFT JOIN produto_pedido AS prod
	ON prod.cod_pedido = k.cod
    WHERE is_troca = 0
	GROUP BY cpf_funcionario
	ORDER BY Lucro
	LIMIT 1
) AS t;


-- Questão 12
UPDATE funcionario
SET salario = salario * 1.06
WHERE cpf IN (
SELECT cpf_funcionario
	FROM (SELECT cpf_funcionario, SUM(valor_venda) - SUM(valor_custo) AS Lucro
			FROM (SELECT cod, cpf_funcionario
			FROM pedido AS p
			LEFT JOIN funcionario AS f
			ON p.cpf_funcionario = f.cpf
			WHERE data_pedido > NOW() - INTERVAL 1 MONTH
		) AS k
		LEFT JOIN produto_pedido AS prod
		ON prod.cod_pedido = k.cod
		GROUP BY cpf_funcionario
		ORDER BY Lucro DESC
		LIMIT 1
	) AS t
);
 
 
-- Questão 13


-- Questão 14
SELECT cpf_cliente, SUM(valor_venda) AS Gasto
FROM pedido AS ped
LEFT JOIN produto_pedido AS prod
ON ped.cod = prod.cod_pedido
GROUP BY cpf_cliente
ORDER BY Gasto DESC;


-- Questão 15



