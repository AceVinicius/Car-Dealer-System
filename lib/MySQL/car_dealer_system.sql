DROP DATABASE IF EXISTS car_dealer_system;
CREATE DATABASE car_dealer_system;
USE car_dealer_system;



CREATE TABLE agencia
(
    cod  INTEGER     NOT NULL AUTO_INCREMENT,
    nome VARCHAR(32) NOT NULL,

    PRIMARY KEY (cod)
);

CREATE TABLE setor
(
    cod         INTEGER     NOT NULL AUTO_INCREMENT,
    cod_agencia INTEGER     NOT NULL,
    nome        VARCHAR(16) NOT NULL,

    PRIMARY KEY (cod),
    
    FOREIGN KEY (cod_agencia) REFERENCES agencia(cod)
);

CREATE TABLE funcionario
(
    cpf           VARCHAR(14) NOT NULL,
    nome          VARCHAR(32) NOT NULL,
    endereco      VARCHAR(64) NOT NULL,
    salario       DOUBLE      NOT NULL,
    cod_setor     INTEGER     NOT NULL,
    data_admissao DATETIME    NOT NULL DEFAULT NOW(),
    data_demissao DATETIME    NULL,

    PRIMARY KEY (cpf),
    
    FOREIGN KEY (cod_setor) REFERENCES setor(cod)
);

CREATE TABLE cliente
(
    cpf              VARCHAR(14) NOT NULL,
    nome             VARCHAR(32) NOT NULL,
    endereco         VARCHAR(64) NOT NULL,
    telefone         VARCHAR(15) NOT NULL,
    placa_carro      VARCHAR(8)  NULL,
    cpf_funcionario  VARCHAR(14) NOT NULL,
    cod_setor_origem INTEGER     NOT NULL,

    PRIMARY KEY (cpf),
    
    FOREIGN KEY (cpf_funcionario)  REFERENCES funcionario(cpf),
    FOREIGN KEY (cod_setor_origem) REFERENCES setor(cod)
);

CREATE TABLE pedido
(
    cod             INTEGER     NOT NULL AUTO_INCREMENT,
    cod_setor       INTEGER     NOT NULL,
    cpf_cliente     VARCHAR(14) NOT NULL,
    cpf_funcionario VARCHAR(14) NOT NULL,
    data_pedido     DATETIME    NOT NULL DEFAULT NOW(),
    valor_pedido    DOUBLE      NOT NULL,

    PRIMARY KEY (cod),

    FOREIGN KEY (cod_setor)       REFERENCES setor(cod),
    FOREIGN KEY (cpf_cliente)     REFERENCES cliente(cpf),
    FOREIGN KEY (cpf_funcionario) REFERENCES funcionario(cpf)
);

CREATE TABLE modelo
(
    cod    INTEGER     NOT NULL AUTO_INCREMENT,
    modelo VARCHAR(32) NOT NULL,

    PRIMARY KEY (cod)
);

CREATE TABLE carro
(
    chassi      VARCHAR(17) NOT NULL,
    marca       VARCHAR(32) NOT NULL,
    ano         INTEGER     NOT NULL,
    valor_custo DOUBLE      NOT NULL,
    valor_venda DOUBLE      NOT NULL,
    is_venda    INTEGER     NOT NULL,
    is_novo     INTEGER     NOT NULL,
    is_troca    INTEGER     NOT NULL,
    cod_setor   INTEGER     NOT NULL,
    cod_modelo  INTEGER     NOT NULL ON DELETE CASCADE,

    PRIMARY KEY (chassi),
    
    FOREIGN KEY (cod_setor)  REFERENCES setor(cod),
    FOREIGN KEY (cod_modelo) REFERENCES modelo(cod)
);

CREATE TABLE produto_pedido
(
    cod           INTEGER     NOT NULL AUTO_INCREMENT,
    placa_veiculo VARCHAR(8)  NOT NULL,
    valor_venda   DOUBLE      NOT NULL,
    valor_custo   DOUBLE      NOT NULL,
    is_troca      INTEGER     NOT NULL,
    cod_pedido    INTEGER     NOT NULL,
    chassi        VARCHAR(17) NOT NULL,

    PRIMARY KEY (cod),

    FOREIGN KEY (cod_pedido) REFERENCES pedido(cod),
    FOREIGN KEY (chassi)     REFERENCES carro(chassi)
);

CREATE TABLE agendamento_revisao
(
    cod                INTEGER  NOT NULL AUTO_INCREMENT,
    data_agendamento   DATETIME NOT NULL,
    is_realizada       INTEGER  NOT NULL,
    cod_produto_pedido INTEGER  NOT NULL,
    
    PRIMARY KEY (cod),
    
    FOREIGN KEY (cod_produto_pedido) REFERENCES produto_pedido(cod)
);


-- TRIGGERS

UPDATE produto_pedido SET valor_venda = valor_custo * 1.10;