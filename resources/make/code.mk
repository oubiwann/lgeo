.PHONY: build repl check check-quiet clean

build:
	rebar3 compile

repl:
	@$(LFE)

check:
	rebar3 eunit -v

check-quiet:
	rebar3 eunit

clean:
	rebar3 clean
	rm -rf priv

dialyzer-build: build
	dialyzer --build_plt --output_plt erlogr.plt \
	-o dialyzer.build \
	-r src ebin

