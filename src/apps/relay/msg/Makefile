MSGFMT=/usr/local/bin/msgfmt
MSGMERGE=/usr/local/bin/msgmerge
LANGUAGES=ko jp

all: languages

languages: relay.po
	@for i in $(LANGUAGES); do \
		echo "--> $(SDP)$$i"; \
		if [ -d $$i ]; then \
			if [ -f $$i/relay.po ]; then \
				$(MSGMERGE) -o $$i/relay.po \
						$$i/relay.po relay.po; \
				$(MSGFMT) -o $$i/relay.mo --statistics \
							$$i/relay.po; \
				if [ -f $$i/relay.mo ]; then \
					mkdir -p $(HOME)/locale/$$i/LC_MESSAGES; \
					cp $$i/relay.mo \
							$(HOME)/locale/$$i/LC_MESSAGES; \
					echo "copy $$i/relay.mo"; \
				fi; \
			else  \
				echo "copy relay.po"; \
				cp -f relay.po $$i; \
			fi; \
		else \
			mkdir -p $$i; \
			cp relay.po $$i; \
		fi; \
		echo "<-- $(SDP)$$i"; \
	done
