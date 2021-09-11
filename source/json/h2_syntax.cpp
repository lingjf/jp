struct h2_json_syntax {
   int i = 0;
   const std::vector<h2_string>& lexical;
   h2_json_syntax(const std::vector<h2_string>& _lexical) : lexical(_lexical) {}

   bool parse(h2_json_node& root_node)
   {
      if (!parse_value(root_node)) return false;
      return lexical.size() <= i;  // nothing more, "{},{}"
   }

   h2_string& filter_string(h2_string& s)
   {
      if (s.enclosed('\"'))
         s = s.unquote('\"');
      else if (s.enclosed('\''))
         s = s.unquote('\'');
      s = s.unescape();
      return s;
   }

   bool requires(const char* s)
   {
      if (lexical.size() <= i || !lexical[i].equals(s)) return false;
      ++i;
      return true;
   }

   bool parse_value(h2_json_node& node)
   {
      if (lexical.size() <= i) return true;

      if (lexical[i].equals("null")) {
         ++i;
         node.type = h2_json_node::t_null;
         return true;
      }
      if (lexical[i].equals("false")) {
         ++i;
         node.type = h2_json_node::t_boolean;
         node.value_boolean = false;
         return true;
      }
      if (lexical[i].equals("true")) {
         ++i;
         node.type = h2_json_node::t_boolean;
         node.value_boolean = true;
         return true;
      }
      if (lexical[i].equals("[")) return parse_array(node);
      if (lexical[i].equals("{")) return parse_object(node);

      if (lexical[i].equals(":")) return false;
      if (lexical[i].equals(",")) return false;

      return parse_string_or_number(node);
   }

   bool parse_key(h2_json_node& node)
   {
      node.key_string = lexical[i++];
      filter_string(node.key_string);
      return true;
   }

   bool parse_string_or_number(h2_json_node& node)
   {
      node.value_string = lexical[i++];
      if (node.value_string.startswith("\"")) {
         filter_string(node.value_string);
         node.type = h2_json_node::t_string;
      } else {
         node.value_double = parse_number(node.value_string.c_str());
         node.type = h2_json_node::t_number;
         node.value_string = "";
      }
      return true;
   }

   bool parse_array(h2_json_node& node)
   {
      if (!requires("[")) return false;
      while (i < lexical.size() && !lexical[i].equals("]")) {
         h2_json_node* new_node = new h2_json_node();
         node.children.push_back(new_node->x);
         if (!parse_value(*new_node)) return false;
         if (i < lexical.size() && lexical[i].equals(","))
            i++;
         else
            break;
      }

      if (!requires("]")) return false;
      node.type = h2_json_node::t_array;
      return true;
   }

   bool parse_object(h2_json_node& node)
   {
      if (!requires("{")) return false;
      while (i < lexical.size() && !lexical[i].equals("}")) {
         h2_json_node* new_node = new h2_json_node();
         node.children.push_back(new_node->x);
         if (!parse_key(*new_node)) return false;
         if (!requires(":")) return false;
         if (!parse_value(*new_node)) return false;
         if (i < lexical.size() && lexical[i].equals(","))
            ++i;
         else
            break;
      }

      if (!requires("}")) return false;
      node.type = h2_json_node::t_object;
      return true;
   }

   double parse_number(const char* num)
   {
      double n = 0, sign = 1, scale = 0;
      int subscale = 0, signsubscale = 1;

      if (*num == '-') {
         sign = -1;
         num++; /* Has sign? */
      }
      if (*num == '0') {
         num++; /* is zero */
      }
      if (*num >= '1' && *num <= '9') {
         do {
            n = (n * 10.0) + (*num - '0');
            num++;
         } while (*num >= '0' && *num <= '9'); /* Number? */
      }

      if (*num == '.' && num[1] >= '0' && num[1] <= '9') {
         num++;
         do {
            n = (n * 10.0) + (*num - '0');
            scale--;
            num++;
         } while (*num >= '0' && *num <= '9');
      } /* Fractional part? */

      if (*num == 'e' || *num == 'E') /* Exponent? */ {
         num++;
         if (*num == '+') {
            num++;
         } else if (*num == '-') {
            signsubscale = -1;
            num++; /* With sign? */
         }
         while (*num >= '0' && *num <= '9') {
            subscale = (subscale * 10) + (*num - '0'); /* Number? */
            num++;
         }
      }

      n = sign * n * pow(10.0, (scale + subscale * signsubscale)); /* number = +/- number.fraction * 10^+/- exponent */

      return n;
   }
};
