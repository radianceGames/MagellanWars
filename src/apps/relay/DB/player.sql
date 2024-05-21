DROP TABLE IF EXISTS Player;

CREATE TABLE Player (
  ID int(10) unsigned DEFAULT '0' NOT NULL,
  Council int(10),
  ClusterOfStars int(10),
  Server int(10),
  c_time int(10),
  PRIMARY KEY (ID)
);
