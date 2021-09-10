struct h2_option
{
  h2_singleton(h2_option);
  bool caseless = false;
  bool fold_json = true;
};

static const h2_option& O = h2_option::I(); // for pretty
